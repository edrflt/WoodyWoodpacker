#include "woody.h"

/*
 * find gap function
 * return the size of the gap in case of sucess, return 0 otherwise
*/
int			off_next_sect(t_pars *pam, EHDR hdr, EPHR *phdr)
{
	EPHR	*actual;
	EPHR	*closest = NULL;

	for (int i = 0; i < hdr.e_phnum; i++)
	{
		actual = (EPHR*)(pam->content + hdr.e_phoff + hdr.e_phentsize * i);
		if (actual->p_offset > phdr->p_offset)
		{
			if (!closest || actual->p_offset < closest->p_offset)
				closest = actual;
		}
	}
	return (closest->p_offset - (phdr->p_offset + phdr->p_filesz));
}

void		section_sz(t_pars *pam)
{
	char	*name;
	EHDR	hdr;
	ESHR	*shdr;
	ESHR	*strtab;

	hdr = pam->hdr;
	strtab = (ESHR*)(pam->content + hdr.e_shoff + hdr.e_shentsize * hdr.e_shstrndx);
	for (int i = 0; i < hdr.e_shnum; i++)
	{
		shdr = (ESHR*)(pam->content + hdr.e_shoff + hdr.e_shentsize * i);
		name = (char *)(pam->content + strtab->sh_offset + shdr->sh_name);
		if (ft_strcmp(name, ".fini") == 0)
		{
			shdr->sh_size += GSIZE + 0x1f;
		}
	}
}

int			find_gap(t_pars *pam, EHDR hdr)
{
	int				size_gap;
	EPHR			*phdr;
	int				end_seg;
	unsigned int	closest_start;

	size_gap = 0;
	end_seg = 0;
	closest_start = 0;
	pam->new_seg = NULL;
	for (int i = 0; i < hdr.e_phnum; i++)
	{
		phdr = (EPHR*)(pam->content + hdr.e_phoff + hdr.e_phentsize * i);
		if (phdr->p_type == PT_LOAD && (phdr->p_flags & PF_X))
		{
			pam->seg = *phdr;
			end_seg = phdr->p_offset + phdr->p_filesz;
			if ((size_gap = off_next_sect(pam, hdr, phdr)) >= GSIZE)
			{
				section_sz(pam);
				phdr->p_memsz += GSIZE + 0x1f;
				phdr->p_filesz += GSIZE + 0x1f;
			}
			pam->off_gap = end_seg + 1;
			if (!(phdr->p_flags & PF_W))
				phdr->p_flags += PF_W;
			return (size_gap);
		}
		else if (end_seg)
		{
			if (!closest_start || (pam->seg.p_offset < phdr->p_offset && closest_start > phdr->p_offset))
				closest_start = phdr->p_offset;
		}
	}
	if (end_seg && closest_start)
	{
		size_gap = closest_start - end_seg;
		pam->off_gap = end_seg + 1;
	}
	else
		printf("Gap error raised\n");
	return (size_gap);
}
