#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <map>

#include "libdwarf.h"
#include "dwarfparser.h"

static void parse_dwarf(char * filepath);
static void read_cu_list(Dwarf_Debug dbg);
static void get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die, int in_level, struct srcfilesdata *sf);
static void resetsrcfiles(Dwarf_Debug dbg, struct srcfilesdata *sf);

// map from address to lineNum and Col
typedef std::map<long unsigned, std::pair<unsigned long, unsigned long> > t_addr2lineAndColMap;
static std::map<std::string, t_addr2lineAndColMap> fileToMap;

struct srcfilesdata {
  char ** srcfiles;
  Dwarf_Signed srcfilescount;
  int srcfilesres;
};

#if 0
void handle_elf(const char* name, FILE* outfile)
{
  int i; 
  int fd = open(name, O_RDONLY, 0); 
  unsigned long entry_addr = 0;
  unsigned long sec_size = 0;

  elf_version(EV_CURRENT);
  ELF* e = elf_begin(fd, ELF_C_READ, NULL);
  ELF64_Ehdr* ehdr = elf64_getehdr(e);
  entry_addr = ehdr->e_entry;
  ELF_Scn* scn = NULL;
  size_t shstrndx;

  elf_getshstrndx(e, &shstrndx);
  while ((scn = elf_nextscn(e, scn)) != NULL) 
  {
    char* sec_name;
    Elf64_Shdr* shdr = elf64_getshdr(scn);
    if ((sec_name = elf_strptr(e, shstrndx, shdr->sh_name)) == NULL)
      printf("error reading name!\n");

    if( !strcmp(sec_name,".text") )
      sec_size = shdr->sh_size;
  }

  printf(stderr,"Scan between %lx to %lx\n", entry_addr, entry_addr + sec_size);
  printf(outfile,"%lx %lx\n", entry_addr, entry_addr+sec_size);
  RangeBottom = entry_addr;
  RangeTop = entry_addr + sec_size;
}
#endif

void parse_dwarf(char* filepath)
{
  int fd = -1;
  fd = open(filepath,O_RDONLY);
  if(fd < 0) 
  {
    printf("Failure attempting to open \"%s\"\n",filepath);
    exit(1);
  }

  Dwarf_Handler errhand = 0;
  Dwarf_Ptr errarg = 0;
  Dwarf_Debug dbg = 0;
  Dwarf_Error error;
  int res = DW_DLV_ERROR;
  res = dwarf_init(fd, DW_DLC_READ, errhand, errarg, &dbg, &error);
  if(res != DW_DLV_OK) 
  {
    printf("Giving up, cannot do DWARF processing\n");
    exit(1);
  }

  read_cu_list(dbg);
  res = dwarf_finish(dbg,&error);
  if(res != DW_DLV_OK) 
  {
    printf("dwarf_finish failed!\n");
    exit(1);
  }
  close(fd);
}

void dump_dwarf(FILE* outfile)
{
  std::map<std::string, t_addr2lineAndColMap>::iterator iter;
  for(iter = fileToMap.begin(); iter != fileToMap.end(); ++iter)
  {
    std::map<long unsigned, std::pair<unsigned long, unsigned long> >::iterator iter2;
    //how much do we try and look for the original source
    unsigned countDown = 0xff;
    t_addr2lineAndColMap fileMap = iter->second;
    for(iter2 = fileMap.begin(); iter2 != fileMap.end(); ++iter2)
    { 
      unsigned long pc = iter2->first;
      fprintf(outfile,"0x%lx, %s, %ld, %ld\n",pc, iter->first.c_str(), fileMap[pc].first, fileMap[pc].second);
    }
  }
}

static void read_cu_list(Dwarf_Debug dbg)
{
  Dwarf_Unsigned cu_header_length = 0;
  Dwarf_Half version_stamp = 0;
  Dwarf_Unsigned abbrev_offset = 0;
  Dwarf_Half address_size = 0;
  Dwarf_Unsigned next_cu_header = 0;
  Dwarf_Error error;
  int cu_number = 0;
  for(;;++cu_number) {
    struct srcfilesdata sf;
    sf.srcfilesres = DW_DLV_ERROR;
    sf.srcfiles = 0;
    sf.srcfilescount = 0;
    Dwarf_Die no_die = 0;
    Dwarf_Die cu_die = 0;
    int res = DW_DLV_ERROR;
    res = dwarf_next_cu_header(dbg,&cu_header_length,
        &version_stamp, &abbrev_offset, &address_size,
        &next_cu_header, &error);
    if(res == DW_DLV_ERROR) 
    {
      printf("Error in dwarf_next_cu_header\n");
      exit(1);
    }
    if(res == DW_DLV_NO_ENTRY) 
    {
      /* Done. */
      return;
    }
    /* The CU will have a single sibling, a cu_die. */
    res = dwarf_siblingof(dbg,no_die,&cu_die,&error);
    if(res == DW_DLV_ERROR) 
    {
      printf("Error in dwarf_siblingof on CU die \n");
      exit(1);
    }

    if(res == DW_DLV_NO_ENTRY) {
      /* Impossible case. */
      printf("no entry! in dwarf_siblingof on CU die \n");
      exit(1);
    }
    get_die_and_siblings(dbg,cu_die,0,&sf);
    dwarf_dealloc(dbg,cu_die,DW_DLA_DIE);
    resetsrcfiles(dbg,&sf);
  }
}

static void list_func_in_die(Dwarf_Debug dbg, Dwarf_Die cu_die)
{
  Dwarf_Signed linecount = 0;
  Dwarf_Line *linebuf = NULL;
  Dwarf_Signed i = 0;
  Dwarf_Addr pc = 0;
  Dwarf_Unsigned lineno = 0;
  Dwarf_Unsigned column = 0;

  Dwarf_Bool newstatement = 0;
  Dwarf_Bool lineendsequence = 0;
  Dwarf_Bool new_basic_block = 0;
  int lres = 0;
  int sres = 0;
  int ares = 0;
  int lires = 0;
  int cores = 0;
  int line_errs = 0;
  Dwarf_Error err;
  lres = dwarf_srclines(cu_die, &linebuf, &linecount, &err);
  if (lres == DW_DLV_ERROR) 
  {
    /* Do not terminate processing */
    printf("DW_DLV_ERROR\n");
  } 
  else if (lres == DW_DLV_NO_ENTRY) 
  {
    /* no line information is included */
  } 
  else 
  {
    char *padding;
    for(Dwarf_Signed i = 0; i < linecount;i++)
    { 
      Dwarf_Line line = linebuf[i];
      char *filenamearg = 0;

      bool found_line_error = false;
      Dwarf_Bool has_is_addr_set = 0;
      int sres = dwarf_linesrc(line, &filenamearg, &err);
      int ares = dwarf_lineaddr(line, &pc, &err);
      int lires = dwarf_lineno(line, &lineno, &err);
      int cores = dwarf_lineoff_b(line, &column, &err);
      std::string fileName(filenamearg);

      if(fileToMap.find(fileName) == fileToMap.end())
      { 
        t_addr2lineAndColMap theMap;
        fileToMap[fileName] = theMap;
      }

      //first time this file has been encountered
      //The map doesnt get initialized
      t_addr2lineAndColMap theMap  = fileToMap[fileName];
      std::pair<unsigned long, unsigned long>lineNoAndCol;

      if(theMap.find(pc) == theMap.end())
      { 
        lineNoAndCol.first = lineno;
        lineNoAndCol.second = column;
        theMap[pc] = lineNoAndCol;
      }
      fileToMap[fileName] = theMap;

    }
    dwarf_srclines_dealloc(dbg, linebuf, linecount);
  }
}

static void get_die_and_siblings(Dwarf_Debug dbg, Dwarf_Die in_die, 
    int in_level, struct srcfilesdata *sf)
{
  int res = DW_DLV_ERROR;
  Dwarf_Die cur_die=in_die;
  Dwarf_Die child = 0;
  Dwarf_Error error;
  list_func_in_die(dbg, in_die);
  //    print_die_data(dbg,in_die,in_level,sf);
  for(;;) {
    Dwarf_Die sib_die = 0;
    res = dwarf_child(cur_die,&child,&error);
    list_func_in_die(dbg, cur_die);
    if(res == DW_DLV_ERROR) {
      printf("Error in dwarf_child , level %d \n",in_level);
      exit(1);
    }
    if(res == DW_DLV_OK) {
      get_die_and_siblings(dbg,child,in_level+1,sf);
    }
    /* res == DW_DLV_NO_ENTRY */
    res = dwarf_siblingof(dbg,cur_die,&sib_die,&error);
    if(res == DW_DLV_ERROR) {
      printf("Error in dwarf_siblingof , level %d \n",in_level);
      exit(1);
    }
    if(res == DW_DLV_NO_ENTRY) {
      /* Done at this level. */
      break;
    }
    /* res == DW_DLV_OK */
    if(cur_die != in_die) {
      dwarf_dealloc(dbg,cur_die,DW_DLA_DIE);
    }
    cur_die = sib_die;

    //print_die_data(dbg,cur_die,in_level,sf);
  }
  return;
}

static void get_addr(Dwarf_Attribute attr,Dwarf_Addr *val)
{
  Dwarf_Error error = 0;
  int res;
  Dwarf_Addr uval = 0;
  res = dwarf_formaddr(attr,&uval,&error);
  if(res == DW_DLV_OK) {
    *val = uval;
    return;
  }
  return;
}

static void get_number(Dwarf_Attribute attr,Dwarf_Unsigned *val)
{
  Dwarf_Error error = 0;
  int res;
  Dwarf_Signed sval = 0;
  Dwarf_Unsigned uval = 0;
  res = dwarf_formudata(attr,&uval,&error);
  if(res == DW_DLV_OK) {
    *val = uval;
    return;
  }
  res = dwarf_formsdata(attr,&sval,&error);
  if(res == DW_DLV_OK) {
    *val = sval;
    return;
  }
  return;
}

static void resetsrcfiles(Dwarf_Debug dbg,struct srcfilesdata *sf)
{
  Dwarf_Signed sri = 0;
  for (sri = 0; sri < sf->srcfilescount; ++sri) {
    dwarf_dealloc(dbg, sf->srcfiles[sri], DW_DLA_STRING);
  }
  dwarf_dealloc(dbg, sf->srcfiles, DW_DLA_LIST);
  sf->srcfilesres = DW_DLV_ERROR;
  sf->srcfiles = 0;
  sf->srcfilescount = 0;
}

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    fprintf(stderr, "Usage: dwarfparser [input binary file name] [output file name]\n");
    exit(1);
  }

  // argv[1]: input binary file name
  // argv[2]: output file name
  parse_dwarf(argv[1]);
  FILE* outfile = fopen(argv[2], "w");
  dump_dwarf(outfile);
  fclose(outfile);
}
