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
    /*
    if (!parent->is_directory) {
        return NULL;
    }

    
    struct inode* new = malloc(sizeof(struct inode));

    new->id = ++max_id;
    new->name = malloc(strlen(name) + 1);
    strcopy(new->name, name);
    new->is_directory = 0;
    new->is_readonly = 0;
    new->filesize = (uint32_t)size_in_bytes;
    
    //Forsøker å opprette entries

    //Oppretter foreldre-forhold
    */

    fprintf( stderr, "%s is not implemented\n", __FUNCTION__ );
    return NULL;
}

struct inode* create_dir( struct inode* parent, const char* name )
{
    if (!parent->is_directory) {
        return NULL;
    }

    //Sjekker om mappe med samme navn finnes fra før
    for (uint32_t i = 0; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        if (strcmp(child->name, name) == 0) {
            return NULL;
        }
    }

    struct inode* new = malloc(sizeof(struct inode))
    ;
    new->id = ++max_id;
    new->name = malloc(strlen(name) + 1);
    strcpy(new->name, name);
    new->is_directory = 1;
    new->is_readonly = 0;
    new->filesize = 0;
    new->num_entries = 0;
    new->entries = NULL; //Tom directory, reallokeres hvis noe skal legges til.

    parent->entries = realloc(parent->entries, (parent->num_entries + 1) * sizeof(uintptr_t));
    parent->entries[parent->num_entries] = (uintptr_t)new;
    parent->num_entries ++;

    return new;
}

struct inode* find_inode_by_name( struct inode* parent, const char* name )
{
    if (!parent->is_directory) {
        return NULL;
    }
    for (uint32_t i = 0; i < parent->num_entries; i++) {
        struct inode* child = (struct inode*)parent->entries[i];
        char* child_name = child->name;
        if (strcmp(child_name, name) == 0) {
            return child;
        }
    }
    return NULL;
}

int delete_file( struct inode* parent, struct inode* node )
{
    fprintf( stderr, "%s is not implemented\n", __FUNCTION__ );
    return -1;
}

int delete_dir( struct inode* parent, struct inode* node )
{
    fprintf( stderr, "%s is not implemented\n", __FUNCTION__ );
    return -1;
}

void save_inodes( const char* master_file_table, struct inode* root )
{
    if (root == NULL) {
        return;
    }
    
    FILE* file = fopen(master_file_table, "wb");

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
            uint64_t child_id64 = child->id;
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

        //For filer er hver entry delt inn i block, extent-par - men siden det lagres
        //som en rekke integers kan vi bare lagre hele verdien som en lang blokk :)
        size_t total_entries_size = sizeof(uint32_t) * node->num_entries * 2;
        fwrite(node->entries, total_entries_size, 1, file);
    }
    return;
}

//Antar korrekt input i binærfilen, sløyfer derfor null-checks
//da jeg antar det er utenfor skopet til oppgaven.
struct inode* load_inodes( const char* master_file_table )
{
    FILE *file = fopen(master_file_table, "rb");

    //lager en dynamisk array av pekere til pekere til inoder
    //Merk at disse først initialiseres med binær-data som entries, 
    //for så å erstattes med pekere etter alle nodene er lest inn
    struct inode** inodes = NULL;
    size_t inodes_size = 0;
    size_t inodes_cap = 0;  //array kapasitet i minnet

    struct inode* root = NULL;
    //note to self: one uint32_t has the size of 4 bytes.
    uint32_t id;

    //bryter ut av løkken hvis forsøk på å lese ID feiler (slutten på filen)
    while (fread(&id, sizeof(uint32_t), 1, file) == 1)
    {
        struct inode *current = malloc(sizeof(struct inode));

        current->id = id;

        //Oppdaterer global id
        if current->id > max_id {
            max_id = current->id;
        }

        //note to self: husk å bruke & i fwrite fordi fwrite kreveren addresse, ikke en verdi.
        //Hvis variabelen allerede er en peker, som name, trenger du ikke &.
        uint32_t name_length;   //lengden i seg selv skal ikke lagres
        fread(&name_length, sizeof(uint32_t), 1, file);

        current->name = malloc(name_length);
        
        fread(current->name, name_length, 1, file);
        fread(&current->is_directory, sizeof(char), 1, file);
        fread(&current->is_readonly, sizeof(char), 1, file);

        if (current->is_directory) {
            current->filesize = 0;

            fread(&current->num_entries, sizeof(uint32_t), 1, file);
            
            //Merk: lagrer entries som bits foreløpig - erstattes med pekere til barn
            //i det ferdige filsystemet.
            current->entries = malloc(current->num_entries * sizeof(uintptr_t));
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

            //I en fil ebstår hver entry av par med blockno(32bit) og extent(32bit)
            size_t total_entries_size = 2 * current->num_entries * sizeof(uint32_t);            
            current->entries = malloc(total_entries_size);

            //leser inn alle entries som én stor blokk (uintptr_t), med størrelse total_entries_size
            fread(current->entries, total_entries_size, 1, file);
        }

        //legger til den nye inoden i arrayet inodes. 
        //Hvis arrayet er på maks kapasitet, doble størrelsen på arrayet.
        if (inodes_size == inodes_cap) {
            if (inodes_size == 0) {
                inodes_cap = 1;
            } else {
                inodes_cap = inodes_cap * 2;
            }
            inodes = realloc(inodes, inodes_cap * sizeof(struct inode*));
        }
        inodes[inodes_size++] = current;
    }

    fclose(file);

    //identifisere rotnoden, i tilfelle den ikke leses inn først i binærfilen
    for (size_t i = 0; i < inodes_size; i++) {
        struct inode* node = inodes[i];
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
                uint64_t child_id = node->entries[j];

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
    fprintf( stderr, "%s is not implemented\n", __FUNCTION__ );
    return;
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
    for( int i=0; i<indent; i++ )
        printf("  ");
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
        uint32_t* extents = (uint32_t*)node->entries;

        for( int i=0; i<node->num_entries; i++ )
        {
            for( int j=0; j<extents[2*i+1]; j++ )
            {
                table[ extents[2*i]+j ] = 1;
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