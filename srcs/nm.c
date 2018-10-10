#include <ft_nm_otool.h>

// #define N_UNDF 0
// #define N_ABS 2
// #define N_TEXT 4
// #define N_DATA 6
// #define N_BSS 8
// #define N_FN 15
// #define N_PBUD 12
// #define N_SECT 14
#define CHARS "1U234567891234tT"

char	get_char_type(char c)
{
	// c & N_TYPE
	/*
	case N_UNDF:
		c = 'u';
		if(symbols[i].nl.n_value != 0)
		    c = 'c';
			break;
    case N_PBUD:
		c = 'u';
		break;
    case N_ABS:
		c = 'a';
		break;
    case N_SECT:
		if(symbols[i].nl.n_sect == process_flags->text_nsect)
		    c = 't';
		else if(symbols[i].nl.n_sect == process_flags->data_nsect)
		    c = 'd';
		else if(symbols[i].nl.n_sect == process_flags->bss_nsect)
		    c = 'b';
		else
		    c = 's';
		break;
    case N_INDR:
		c = 'i';
		break;
    default:
		c = '?';
		break;
	    }
	*/
	char ret;
	if ((c & N_TYPE) == N_UNDF)
		ret = 'u';
	else if ((c & N_TYPE) == N_SECT)
		ret = 't';
	



	// (not)external
	if (c & 0x01)
		ret = ft_toupper(ret);
	return (ret);
}

void	print_output(int nsyms, int symoff, int stroff, char *ptr)
{
	int				i;
	char			*stringtable;
	struct nlist_64	*array;

	array = (void *)ptr + symoff;
	stringtable = (void *)ptr + stroff;
	i = 0;
	while (i < nsyms)
	{
		char c = get_char_type(array[i].n_type);
		ft_printf("%016llx %c %s\n", array[i].n_value, c, stringtable + array[i].n_un.n_strx);
		i++;
	}
}

void	handle_64(char *ptr)
{
	uint32_t				ncmds;
	uint32_t				i;
	struct mach_header_64	*header;
	struct load_command		*lc;
	struct symtab_command	*sym;

	header = (struct mach_header_64 *)ptr;
	ncmds = header->ncmds;
	i = 0;
	lc = (void *)ptr + sizeof(*header);
	while (i < ncmds)
	{
		if (lc->cmd == LC_SYMTAB)
		{
			sym = (struct symtab_command *)lc;
			print_output(sym->nsyms, sym->symoff, sym->stroff, ptr);
			break ;
		}
		lc = (void *)lc + lc->cmdsize;
		i++;
	}

}

void	nm(char *ptr)
{
	unsigned int	magic_number;

	magic_number = *(int *)ptr;
	if (magic_number == MH_MAGIC_64)
	{
		handle_64(ptr);
	}
}

int		main(int argc, char **argv)
{
	int			fd;
	char		*ptr;
	struct stat	buf;

	if (argc != 2)
	{
		if ((fd = open("./a.out", O_RDONLY)) < 0)
		{
			perror("open ./a.out");
			return (EXIT_FAILURE);
		}
	}
	else if ((fd = open(argv[1], O_RDONLY)) < 0)
	{
		perror("open");
		return (EXIT_FAILURE);
	}

	if (fstat(fd, &buf) < 0)
	{
		perror("fstat");
		return (EXIT_FAILURE);
	}
	if ((ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
	{
		perror("mmap");
		return (EXIT_FAILURE);
	}
	nm(ptr);
	if (munmap(ptr, buf.st_size) < 0)
	{
		perror("munmap");
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
