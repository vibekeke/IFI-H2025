#include "inode.h"
#include "block_allocation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

static int max_id = 0; //I oppgaven står det at hver node skal ha en unik ID, men ikke så mye om hvordan
                //dette skal implementeres. Antar derfor en enkel løsning er tilstrekkelig.
                //Her: en global teller som kun inkrementerer, for hver fil/dir som blir lagt til.

struct inode* create_file( struct inode* parent, const char* name, char readonly, int size_in_bytes )
{

    if (parent == NULL || !parent->is_directory) {
        return NULL;
    }

    if (find_inode_by_name(parent, name) != NULL) {
        return NULL;
    }
    
    struct inode* new = malloc(sizeof(struct inode));
    if (new == NULL) {
        printf("Feil ved allokering av minne i create_file");
        return NULL;
    }

    new->id = ++max_id;
    new->name = malloc(strlen(name) + 1);
    if (new->name == NULL) {
        printf("Feil ved allokering av minne i create_file");
        free(new);
        return NULL;
    }
    strcpy(new->name, name);
    new->is_directory = 0;
    new->is_readonly = readonly;
    new->filesize = (uint32_t)size_in_bytes;
    new->num_entries = 0;
    new->entries = NULL;
    
    //Forsøker å opprette entries

    //Trenger å allokere nok blokker til å dekke hele størrelsen / 4096
    int blocks_to_allocate = (size_in_bytes + 4096 - 1) / 4096;

    struct Extent* extents = NULL;

    uint32_t index = 0;

    while (blocks_to_allocate > 0) {
        int attempted_blocks = 4;
        int success = 0;
        
        if (blocks_to_allocate < 4) {
            attempted_blocks = blocks_to_allocate;
        }

        while (attempted_blocks > 0) {
            int new_block = allocate_blocks(attempted_blocks);
            if (new_block != -1) {
                struct Extent* temp_extents = realloc(extents, (index + 1) * sizeof(struct Extent));
                if (temp_extents == NULL) {
                    printf("Feil ved re-allokering av minne i create_file");
                        
                    //Frigjøre den nåværende blokken
                    for (uint32_t i = 0; i < attempted_blocks; i++) {
                        free_block(new_block + i);
                    }
                    
                    //Frigjøre alle blokker som ble allokert tidligere
                    for (uint32_t i = 0; i < index; i++) {
                        struct Extent* current = &extents[i];
                        for (uint32_t e = 0; e < current->extent; e++) {
                            free_block(current->blockno + e);
                        }
                    }

                    free(extents);
                    free(new->name);
                    free(new);
                    return NULL;
                }
                extents = temp_extents;
                extents[index].blockno = new_block;
                extents[index].extent = attempted_blocks;
                index++;
                blocks_to_allocate -= attempted_blocks;
                success = 1;
                break;
            } else {
                attempted_blocks -= 1;
                continue;
            }
        }
        
        //Hvis vi kom så langt uten å allokere alle blokkene
        //betyr det at det ikke var nok plass
        //på disk til å lagre filen. Vi må da frigjøre alle ressurser.
        if (success != 1) {
            //Frigjøre alle blokker som allerede er allokert.
            for (uint32_t i = 0; i < index; i++) {
                struct Extent* current = &extents[i];
                for (uint32_t e = 0; e < current->extent; e++) {
                    free_block(current->blockno + e);
                }
            }
            free(extents);
            free(new->name);
            free(new);
            return NULL;
        }
    }
    
    new->num_entries = index;
    new->entries = (uintptr_t*)extents;

    //Oppretter foreldre-forhold
    uintptr_t* temp_parent_entries = realloc(parent->entries, (parent->num_entries + 1) * sizeof(uintptr_t));
    if (temp_parent_entries == NULL) {
        printf("Feil ved re-allokering av minne i create_file");
                    //Frigjøre alle blokker som allerede er allokert.
        for (uint32_t i = 0; i < new->num_entries; i++) {
            struct Extent* current = &extents[i];
            for (uint32_t e = 0; e < current->extent; e++) {
                    free_block(current->blockno + e);
                }
        }
        free(extents);
        free(new->name);
        free(new);
        return NULL;
    }
    parent->entries = temp_parent_entries;
    parent->entries[parent->num_entries] = (uintptr_t)new;
    parent->num_entries++;

    return new;
}

struct inode* create_dir( struct inode* parent, const char* name )
{
    if (parent != NULL && !parent->is_directory) {
        return NULL;
    }

    //Sjekker om mappe med samme navn finnes fra før
    for (uint32_t i = 0; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        if (strcmp(child->name, name) == 0) {
            return NULL;
        }
    }

    struct inode* new = malloc(sizeof(struct inode));
    if (new == NULL) {
        return NULL;
    }

    new->id = ++max_id;
    new->name = malloc(strlen(name) + 1);
    if (new->name == NULL) {
        free(new);
        return NULL;
    }
    strcpy(new->name, name);

    new->is_directory = 1;
    new->is_readonly = 0;
    new->filesize = 0;
    new->num_entries = 0;
    new->entries = NULL; //Tom directory, reallokeres hvis noe skal legges til.

    
    uintptr_t* temp_entries = realloc(parent->entries, (parent->num_entries + 1) * sizeof(uintptr_t));
    if (temp_entries == NULL) {
        free(new->name);
        free(new);
        return NULL;
    }
        parent->entries = temp_entries;
        parent->entries[parent->num_entries] = (uintptr_t)new;
        parent->num_entries++;

    return new;
}

struct inode* find_inode_by_name( struct inode* parent, const char* name )
{
    if (parent == NULL || name == NULL) {
        return NULL;
    }
    if (!parent->is_directory) {
        return NULL;
    }

    //Sjekker om barn er barn av parent.
    for (uint32_t i = 0; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        if (child == NULL) {
            continue;
        }
        char* child_name = child->name;
        if (strcmp(child_name, name) == 0) {
            return child;
        }
    }
    return NULL;
}

int delete_file( struct inode* parent, struct inode* node )
{
    if (node == NULL || node->is_directory) {
    return -1;
    }

    if (parent == NULL || !parent->is_directory) {
        return -1;
    }

    int found_index = -1; //indeksen til barnet i foreldre-listen
    for (uint32_t i = 0; i < parent->num_entries; i++){
        struct inode* child = (struct inode*)parent->entries[i];
        if (child->id == node->id) {
            found_index = i;
            break;
        } 
    }

    //Fjerner noden fra parent's entries liste. 
    if (found_index != -1) {
        for (uint32_t i = 0; i < parent->num_entries - 1; i++) {
            if (i >= found_index) {
            parent->entries[i] = parent->entries[i + 1];
            }
        }
        parent->num_entries -= 1;
        parent->entries = realloc(parent->entries, (parent->num_entries) * sizeof(uintptr_t));
    } else { //parent er ikke forelder til node
        return -1;
    } 

    //Blokknummer er startblokken og ekstent er antall extents
    struct Extent* extents = (struct Extent*)node->entries;
    for (uint32_t i = 0; i < node->num_entries; i++) {
        struct Extent* current = &extents[i];
        for (uint32_t e = 0; e < current->extent; e++) {
            int try = free_block(current->blockno + e);
            if (try == -1) {
                return -1;
            }
        }
    }

    free_inode_data(node);

    return 0;
}

int delete_dir( struct inode* parent, struct inode* node )
{
    if (node == NULL || node->is_directory == 0) {
        return -1;
    }

    if (node->num_entries != 0) {
        return -1;
    }

    if (parent == NULL || !parent->is_directory) {
        return -1;
    }
    
    //Finner barnets indeks i foreldre-listen.
    int found_index = -1;
    for (uint32_t i = 0; i < parent->num_entries; i++){
        struct inode* child = (struct inode*)parent->entries[i];
        if (child->id == node->id) {
            found_index = i;
            break;
        } 
    }

    //Fjerner noden fra parent's entries liste, og fjerner eventuelt hull
    if (found_index != -1) {
        for (uint32_t i = 0; i < parent->num_entries - 1; i++) {
            if (i >= found_index) {
            parent->entries[i] = parent->entries[i + 1];
            }
        }
        parent->num_entries -= 1;
        uintptr_t* temp_entries = realloc(parent->entries, (parent->num_entries) * sizeof(uintptr_t));
        if (temp_entries == NULL) {
            return -1;
        }
        parent->entries = temp_entries;
    } else {
        //parent er ikke forelder til inoden node
        return -1;
    } 

    //Frigjør data fra minnet.
    free_inode_data(node);

    return 0;
}

void save_inodes( const char* master_file_table, struct inode* root )
{
    if (root == NULL) {
        return;
    }
    
    FILE* file = fopen(master_file_table, "wb");
    
    if (file == NULL) {
        return;
    }

    //Traverserer treet og lagrer til fil ved rekursivt dybde-først søk.
    save_inodes_DFS(file, root);

    fclose(file);

    return;
}

void save_inodes_DFS(FILE* file, struct inode* node) {
    if (node == NULL) {
        return;
    }

    //Skriver felles informasjon til fil
    fwrite(&node->id, sizeof(uint32_t), 1, file);

    uint32_t name_length = (strlen(node->name) + 1); //viktig å huske ekstra plass for \0
    fwrite(&name_length, sizeof(uint32_t), 1, file);
    fwrite(node->name, name_length, 1, file);

    fwrite(&node->is_directory, sizeof(char), 1, file);
    fwrite(&node->is_readonly, sizeof(char), 1, file);

    if (node->is_directory) {
        fwrite(&node->num_entries, sizeof(uint32_t), 1, file);

        //Itererer gjennom barna for å lagre barne-verdier (id)
        for (uint32_t i = 0; i < node->num_entries; i++) {
            struct inode* child = (struct inode*)node->entries[i];  //må caste fra uintpr_t
            uint64_t child_id64 = (uint64_t)child->id;
            fwrite(&child_id64, sizeof(uint64_t), 1, file);
        }
        //Itererer gjennom barna for å behandle ytterligere noder.
        for (uint32_t i = 0; i < node->num_entries; i++) {
            struct inode* child = (struct inode*)node->entries[i];
            save_inodes_DFS(file, child);
        }
    } else {
        fwrite(&node->filesize, sizeof(uint32_t), 1, file);

        fwrite(&node->num_entries, sizeof(uint32_t), 1, file);

        struct Extent* extents = (struct Extent*)node->entries;
        for (uint32_t i = 0; i < node->num_entries; i++) {
            fwrite(&extents[i].blockno, sizeof(uint32_t), 1, file);
            fwrite(&extents[i].extent, sizeof(uint32_t), 1, file);
        }
    }
    return;
}

//Antar korrekt input i binærfilen, sløyfer derfor null-checks
//da jeg antar det er utenfor skopet til oppgaven.
struct inode* load_inodes( const char* master_file_table )
{
    FILE *file = fopen(master_file_table, "rb");

    if (file == NULL) {
        return NULL; // file missing / cannot open
    }

    //lager en dynamisk array av pekere til pekere til inoder
    //Merk at disse først initialiseres med binær-data som entries, 
    //for så å erstattes med pekere etter alle nodene er lest inn
    struct inode** inodes = NULL;
    size_t inodes_size = 0;
    size_t inodes_cap = 0;  //array kapasitet i minnet

    struct inode* root = NULL;
    //note to self: one uint32_t has the size of 4 bytes.

    //bryter ut av løkken hvis forsøk på å lese ID feiler (slutten på filen)
    while (1)
    {
        uint32_t id;
        if (fread(&id, sizeof(uint32_t), 1, file) != 1) {
            break;
        }

        struct inode *current = malloc(sizeof(struct inode));
        if (current == NULL) {
            printf("Feil ved allokering av minne i load_inodes");
            free(inodes);
            fclose(file);
            return NULL;
        }

        current->id = id;

        //Oppdaterer global id
        if (current->id > max_id) {
            max_id = current->id;
        }

        //note to self: husk å bruke & i fwrite fordi fwrite kreveren addresse, ikke en verdi.
        //Hvis variabelen allerede er en peker, som name, trenger du ikke &.
        uint32_t name_length;   //lengden i seg selv skal ikke lagres
        fread(&name_length, sizeof(uint32_t), 1, file);

        current->name = malloc(name_length);
        if (current->name == NULL) {
            printf("Feil ved allokering av minne i load_inodes");
            for (size_t i = 0; i < inodes_size; i++) {
                free_inode_data(inodes[i]);
            }
            free(inodes);
            fclose(file);
            return NULL;
        }
        
        fread(current->name, name_length, 1, file);
        fread(&current->is_directory, sizeof(char), 1, file);
        fread(&current->is_readonly, sizeof(char), 1, file);

        if (current->is_directory) {
            current->filesize = 0;

            fread(&current->num_entries, sizeof(uint32_t), 1, file);
            
            //Merk: lagrer entries som bits foreløpig - erstattes med pekere til barn
            //i det ferdige filsystemet.
            current->entries = malloc(current->num_entries * sizeof(uintptr_t));
            if (current->entries == NULL) {
                printf("Feil ved allokering av minne i load_inodes");
                for (size_t i = 0; i < inodes_size; i++) {
                    free_inode_data(inodes[i]);
                }
                free(inodes);
                fclose(file);
                return NULL;
            }
            for (uint32_t i = 0; i < current->num_entries; i++) {
                //ref: på disk er entries serialisert som en sekvens av 64-biters felt, 
                //som inneholder ID-ene til ulike inodes
                uint64_t child_id;  
                fread(&child_id, sizeof(uint64_t), 1, file);
        
                current->entries[i] = (uintptr_t)child_id;
            }
        } else {
            fread(&current->filesize, sizeof(uint32_t), 1, file);
            fread(&current->num_entries, sizeof(uint32_t), 1, file);

            current->entries = malloc(current->num_entries * sizeof(struct Extent));
            if (current->entries == NULL) {
                printf("Feil ved allokering av minne i load_inodes");
                for (size_t i = 0; i < inodes_size; i++) {
                    free_inode_data(inodes[i]);
                }
                free(inodes);
                fclose(file);
                return NULL;
            }
            for (uint32_t i = 0; i < current->num_entries; i++) {
                struct Extent* extent = &((struct Extent*)current->entries)[i];
                fread(&extent->blockno, sizeof(uint32_t), 1, file);
                fread(&extent->extent, sizeof(uint32_t), 1, file);
                
            //antar vi ikke trenger å allokere blokker siden denne operasjonen allerede er gjort ved opprettelse
            }
        }

        //legger til den nye inoden i arrayet inodes. 
        //Hvis arrayet er på maks kapasitet, doble størrelsen på arrayet.
        if (inodes_size == inodes_cap) {
            if (inodes_size == 0) {
                inodes_cap = 1;
            } else {
                inodes_cap = inodes_cap * 2;
            }
            struct inode** temp_inodes = realloc(inodes, inodes_cap * sizeof(struct inode*));
            if (temp_inodes == NULL) {
                printf("Feil ved re-allokering av minne i load_inodes");
                for (size_t i = 0; i < inodes_size; i++) {
                    free_inode_data(inodes[i]);
                }
                free(inodes);
                fclose(file);
                return NULL;
            }
            inodes = temp_inodes;
        }
        inodes[inodes_size++] = current;
    }

    fclose(file);

    //identifisere rotnoden, i tilfelle den ikke leses inn først i binærfilen
    for (size_t i = 0; i < inodes_size; i++) {
        struct inode* node = inodes[i];
        //Antar at rotnoden er den eneste noden som er en directory og har navnet "/". 
        if (node-> is_directory && strcmp(node->name, "/") == 0) {
            root = node;
            break;
        }
    }

    //Itererer gjennom alle inoder for å finne directories
    for (size_t i = 0; i < inodes_size; i++) {
        struct inode* node = inodes[i];

        //Hvis noden er en dir, iterer gjennom alle entries (barn), og erstatt med pekere til de faktiske barna.
        if (node->is_directory) {
            for (uint32_t j = 0; j < node->num_entries; j++) {
                struct inode* child = NULL;
                uint64_t child_id = (uint64_t)(node->entries[j]);

                //Iterer gjennom alle barna for å finne ID som matcher...
                for (size_t k = 0; k < inodes_size; k++) {
                    if (inodes[k]-> id == child_id) {
                        child = inodes[k];
                        break;
                    }
                }
                node->entries[j] = (uintptr_t)child;
            }
        }
    }
    free(inodes);
    return root;
}

void fs_shutdown( struct inode* inode )
{
    free_inode_data(inode);
    return;
}


void free_inode_data(struct inode* node) {
    if (node == NULL) {
        return;
    }

    //Behandle alle barn først dersom noden er en mappe
    if (node->is_directory) {
        for(uint32_t i = 0; i < node->num_entries; i++) {
            free_inode_data((struct inode*)node->entries[i]);
        }
    }
    //Frigjør noden
    free(node->name);
    free(node->entries);
    free(node);
}

/* This static variable is used to change the indentation while debug_fs
 * is walking through the tree of inodes and prints information.
 */
static int indent = 0;

static void debug_fs_print_table( const char* table );
static void debug_fs_tree_walk( struct inode* node, char* table );

void debug_fs( struct inode* node )
{
    char* table = calloc( NUM_BLOCKS, 1 );
    debug_fs_tree_walk( node, table );
    debug_fs_print_table( table );
    free( table );
}

static void debug_fs_tree_walk( struct inode* node, char* table )
{
    if( node == NULL ) return;
    for( int i=0; i<indent; i++ ) {
        printf("  ");
    }
    if( node->is_directory )
    {
        printf("%s (id %d)\n", node->name, node->id );
        indent++;
        for( int i=0; i<node->num_entries; i++ )
        {
            struct inode* child = (struct inode*)node->entries[i];
            debug_fs_tree_walk( child, table );
        }
        indent--;
    }
    else
    {
        printf("%s (id %d size %d)\n", node->name, node->id, node->filesize );

        /* The following is an ugly solution. We expect you to discover a
         * better way of handling extents in the node->entries array, and did
         * it like this because we don't want to give away a good solution here.
         */
        struct Extent* extents = (struct Extent*)node->entries; 
        //Endret debug tree walk til å passe med strukturen til Extent

        for( int i=0; i < node->num_entries; i++ )
        {
            for( int j=0; j < extents[i].extent; j++ )
            {
                table[ extents[i].blockno + j ] = 1;
            }
        }
    }
}

static void debug_fs_print_table( const char* table )
{
    printf("Blocks recorded in master file table:");
    for( int i=0; i<NUM_BLOCKS; i++ )
    {
        if( i % 20 == 0 ) printf("\n%03d: ", i);
        printf("%d", table[i] );
    }
    printf("\n\n");
}