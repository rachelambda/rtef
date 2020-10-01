# Program Structure

* Argument Handler
    * Resolve args

* File validator
    * Check magic and arch

* Information collector 
    * Collect all info from relevant tables and sections in all input files
    * Check for collissions
    * Check for unsupported things (such as dynamic relocation)

* Information merger
    * Resolve missing symbols (relocation)
    * Merge section data
    * Discard data not needed in output file

* Offset and alignment resolver **(IF THERE IS TIME)**
    * Move and align sections to fit the users specification

* File writer
    * Move merged info into properly structured ELF file
    * Construct ELF headers to fit merged info
