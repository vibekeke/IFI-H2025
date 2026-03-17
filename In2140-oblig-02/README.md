## In2140 Oblig-2



## Instruksjoner 
MFT is the name of the master_file_table
BAT is the name of the block allocation table

# Test opprettelse av filsystem
./create_fs_1 my_mft my_bat
./create_fs_2 my_mft my_bat
./create_fs_3 my_mft my_bat

# Test innlasting av filsystem
./load_fs_1 my_mft my_bat
./load_fs_2 my_mft my_bat
./load_fs_3 my_mft my_bat

# Test opprettelse og sletting av filer
.create_and_delete my_mft my_bat
