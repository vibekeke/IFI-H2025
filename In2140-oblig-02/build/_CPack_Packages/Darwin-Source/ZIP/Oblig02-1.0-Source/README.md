# In2140 Oblig-2

## Instruksjoner 
Kjør test-programmer med
./check_disk bat_fil
.check_fs mft_fil
./create_fs_* mft_fil bat_fil
./load_fs_* mft_fil bat_fil
./create_and_delete mft_fil bat_fil

## Hvordan lese fra master file table.
master file table lagret på disk er en binærfil som inneholder inoder av typen dir (directory) og fil (fil). create_fs filene leser disse inn i minnet ved hjelp av block_allocation.c og inode.c (obligens besvarelse) på følgende måte:

Master file table er serialisert som følger, i rekkefølge
- 4 bytes representerer inodens ID
- 4 bytes representerer lengden på inodens navn
- Et ukjent antall bytes (gitt ved forrige 4 bytes) gir en streng som er inodens navn.
- 1 byte representerer is_directory, altså om inoden er en fil
- 1 byte representerer is_readonly, also om inoden er readonly
- Dersom inoden er en fil, vil de neste 4 bytesene representere filens størrelse. Dette uteblir dersom inoden er av typen directory.
- 4 bytes representerer antall "entries", num_entries
- 4 Bytes representerer antall "entries", num_entries

De resterende blokkene avhenger av inodens type.
Directories
- et antall med 4 bytes representerer ID-ene til barne-inoder, som kan være filer og directories. Antallet er gitt ved num_entries.

Filer:
- Et antall med 2 x 4 bytes representerer henholdsvis et blokknummer på disk, og extent. Extent beskriver hvor mange blokker ved siden av hverandre som kan leses ut ifra ett blokknummer på disk. Antall blokknummer/extent par gis av num_entries.

Siden de delene av binærfilen som representerer verdier av ukjent størrelse har egne verdier som beskriver lengden på disse verdiene, kan vi forsikre oss om at dataene blir lest inn riktig. 


Følgende er en mer detaljert gjennomgang av hvordan funksjonen load_inodes() i inode.c implementerer innlastningen:
1. Åpner filen gitt ved filnavnet char* master_file_table
2. Oppretter en dynamisk array med pekere til pekere til inoder inodes, og angir en nåværende størrelse og en makskapasitet til arrayet i minnet. Venter med å allokere plass til nye inoder lastes inn. 
3. Leser fil vha. while-løkke som bryter ut når det ikke er mer data å lese.
4. inode-innlesning -- 
    a. Leser inn en id (4 bytes -> uint32_t). Ved suksess, gå videre.
    b. Allokerer plass i minnet til hele inoden
    c. (Oppdater en global max_id verdi for å sørge for unike ID-er i fremtiden)
    d. Allokerer plass i minnet til navnet, gitt ved navnets størrelse. inode->name_length
    e. Leser inn navnets størrelse (4 bytes -> uint32_t). 
    f. Leser inn et antall bits gitt ved navnets størrelse = inodens navn. inode->name
    g. Leser inn 1 byte som beskriver om filen er en directory (inode->is_directory) og 1 byte som beskriver om filen er read-only (inode->is_readonly)
    h. Les inn 4 bytes som representerer antall entries (inode->num_entries).
    j1. Hvis noden er en directory:
        - Alloker plass til verdiene gitt ved num_entries * sizeof(uintptr_t).
        - Gitt num_entries, les et num_entries antall 8-byte blokker som gir ID-er til barnenoder. NB! Lagrer disse midlertidig kun som uint64, og bytter disse med nodepeker etter fullstendig innlastning.
    j2. Hvis noden er en fil: 
        - Allokerer plass til entries gitt ved num_entries * sizeof(struct Extent).
        Gitt num_entries, les et num_entries antall 8-byte blokker som gir par med block_no (uint32_t) og extent (uint32_t). Disse lagres i en struct Extent som holder ett slikt par hver.
        -> For hvert innleste par, gi disse en ny indeks i current->entries.
    k. Den nye noden kan nå legges til i inodes-arrayet. Hvis arrayet er fullt dobles kapasiteten til arrayet og minnet reallokeres - (leste at er vanlig implementasjon av dynamiske arrays i c). Setter den nye noden inn i den øverste indeksen i arrayet.
-- Avslutning og opprydning --
5. Lukker filen
6. Identifiserer rotnoden ved å løpe gjennom alle inodene og sette rotnoden til directory med navn "/". 
7. Rydder opp i directory-pekere: Løper gjennom alle noder. For alle directories, erstattes pekere til barnenode-ID-er til faktiske barnenoder i minnet. Verdiene må castes til uintptr_t for å holde pekere til barnenodene og ikke en int.
8. Frigjør det dynamiske arrayet inodes, og returner kun rotnoden. Nå eksisterer alt innholdet i master_file_table-filen i minnet.


## Implementeringskrav
Alle testene kjører og valgrind rapporterer 0 lekkasjer da jeg testet programmene.

Alle implementeringskrav definert av oppgaven er oppfylt etter beste evne, men med følgende begrensninger: 

- Antar at inputdata er korrekt og utfører derfor ikke full håndtering for feil i input-filene. Antar at vi kan at vi kan stole på input-filene og test-scriptsene.