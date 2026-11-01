#ifndef NESTFOLDER_SHELL_PROFILES_H
#define NESTFOLDER_SHELL_PROFILES_H

#include <stdint.h>

typedef struct {
	const char *name;
	uint32_t seg0_size;
	uint32_t seg1_size;
	uint32_t folder_open;
	uint32_t folder_close;
	uint32_t fl_add;
	uint32_t fl_del;
	uint32_t gate1;
	uint32_t gate2;
	uint32_t isfolder;
	uint32_t iconcapture;
	uint32_t mkfolderjob;
	uint32_t str_assign;
	uint32_t wstr_assign;
	uint32_t reload;
	uint32_t capfin;
	uint32_t reload_from_ctor;
	uint32_t imgget;
	uint32_t cap_child_ret;
	uint32_t cap_call;
	uint32_t cap_pos;
	uint32_t cap_list;
	uint32_t fldr_param;
	uint32_t paf_free;
	uint32_t seg1_g;
	uint32_t seg1_view;
} shell_profile_t;

static const shell_profile_t shell_profiles[] = {
	{
		.name = "retail 3.60-3.61",
		.seg0_size = 0x00541B74u,
		.seg1_size = 0x00093FACu,
		.folder_open = 0x000AFCD8u,
		.folder_close = 0x000AD274u,
		.fl_add = 0x000D1768u,
		.fl_del = 0x000D18D6u,
		.gate1 = 0x000B5F90u,
		.gate2 = 0x000B63E2u,
		.isfolder = 0x000CF6F4u,
		.iconcapture = 0x000D1C38u,
		.mkfolderjob = 0x001C6B2Au,
		.str_assign = 0x00408E14u,
		.wstr_assign = 0x0040B63Cu,
		.reload = 0x000CF786u,
		.capfin = 0x000B30EEu,
		.reload_from_ctor = 0x000CFCFCu,
		.imgget = 0x000BD34Eu,
		.cap_child_ret = 0x000B81DCu,
		.cap_call = 0x000B81D0u,
		.cap_pos = 0x000B810Au,
		.cap_list = 0x000B80DEu,
		.fldr_param = 0x000CFAAEu,
		.paf_free = 0x0045AA40u,
		.seg1_g = 0x00012144u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "retail 3.63",
		.seg0_size = 0x00541C34u,
		.seg1_size = 0x00093FACu,
		.folder_open = 0x000AFD30u,
		.folder_close = 0x000AD2CCu,
		.fl_add = 0x000D17C0u,
		.fl_del = 0x000D192Eu,
		.gate1 = 0x000B5FE8u,
		.gate2 = 0x000B643Au,
		.isfolder = 0x000CF74Cu,
		.iconcapture = 0x000D1C90u,
		.mkfolderjob = 0x001C6BF2u,
		.str_assign = 0x00408EB4u,
		.wstr_assign = 0x0040B6DCu,
		.reload = 0x000CF7DEu,
		.capfin = 0x000B3146u,
		.reload_from_ctor = 0x000CFD54u,
		.imgget = 0x000BD3A6u,
		.cap_child_ret = 0x000B8234u,
		.cap_call = 0x000B8228u,
		.cap_pos = 0x000B8162u,
		.cap_list = 0x000B8136u,
		.fldr_param = 0x000CFB06u,
		.paf_free = 0x0045AAE0u,
		.seg1_g = 0x00012144u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "retail 3.65-3.68",
		.seg0_size = 0x005420F4u,
		.seg1_size = 0x00093FBCu,
		.folder_open = 0x000AFD30u,
		.folder_close = 0x000AD2CCu,
		.fl_add = 0x000D17C0u,
		.fl_del = 0x000D192Eu,
		.gate1 = 0x000B5FE8u,
		.gate2 = 0x000B643Au,
		.isfolder = 0x000CF74Cu,
		.iconcapture = 0x000D1C90u,
		.mkfolderjob = 0x001C6BF2u,
		.str_assign = 0x0040925Cu,
		.wstr_assign = 0x0040BA84u,
		.reload = 0x000CF7DEu,
		.capfin = 0x000B3146u,
		.reload_from_ctor = 0x000CFD54u,
		.imgget = 0x000BD3A6u,
		.cap_child_ret = 0x000B8234u,
		.cap_call = 0x000B8228u,
		.cap_pos = 0x000B8162u,
		.cap_list = 0x000B8136u,
		.fldr_param = 0x000CFB06u,
		.paf_free = 0x0045AE88u,
		.seg1_g = 0x00012154u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "retail 3.69-3.74",
		.seg0_size = 0x00542134u,
		.seg1_size = 0x00093FBCu,
		.folder_open = 0x000AFD30u,
		.folder_close = 0x000AD2CCu,
		.fl_add = 0x000D17C0u,
		.fl_del = 0x000D192Eu,
		.gate1 = 0x000B5FE8u,
		.gate2 = 0x000B643Au,
		.isfolder = 0x000CF74Cu,
		.iconcapture = 0x000D1C90u,
		.mkfolderjob = 0x001C6BF2u,
		.str_assign = 0x00409280u,
		.wstr_assign = 0x0040BAA8u,
		.reload = 0x000CF7DEu,
		.capfin = 0x000B3146u,
		.reload_from_ctor = 0x000CFD54u,
		.imgget = 0x000BD3A6u,
		.cap_child_ret = 0x000B8234u,
		.cap_call = 0x000B8228u,
		.cap_pos = 0x000B8162u,
		.cap_list = 0x000B8136u,
		.fldr_param = 0x000CFB06u,
		.paf_free = 0x0045AEA8u,
		.seg1_g = 0x00012154u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "devkit 3.600-3.610",
		.seg0_size = 0x00530C74u,
		.seg1_size = 0x00092CB4u,
		.folder_open = 0x000AC3FCu,
		.folder_close = 0x000A95DAu,
		.fl_add = 0x000CE550u,
		.fl_del = 0x000CE6BEu,
		.gate1 = 0x000B2710u,
		.gate2 = 0x000B2B62u,
		.isfolder = 0x000CC264u,
		.iconcapture = 0x000CEA20u,
		.mkfolderjob = 0x001BE5E2u,
		.str_assign = 0x003FAD88u,
		.wstr_assign = 0x003FD5B0u,
		.reload = 0x000CC2F6u,
		.capfin = 0x000AF86Eu,
		.reload_from_ctor = 0x000CC870u,
		.imgget = 0x000B9ACEu,
		.cap_child_ret = 0x000B495Cu,
		.cap_call = 0x000B4950u,
		.cap_pos = 0x000B488Au,
		.cap_list = 0x000B485Eu,
		.fldr_param = 0x000CC61Eu,
		.paf_free = 0x0044C1B8u,
		.seg1_g = 0x00011EB4u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "devkit 3.630",
		.seg0_size = 0x00530D74u,
		.seg1_size = 0x00092CB4u,
		.folder_open = 0x000AC454u,
		.folder_close = 0x000A9632u,
		.fl_add = 0x000CE5A8u,
		.fl_del = 0x000CE716u,
		.gate1 = 0x000B2768u,
		.gate2 = 0x000B2BBAu,
		.isfolder = 0x000CC2BCu,
		.iconcapture = 0x000CEA78u,
		.mkfolderjob = 0x001BE6AAu,
		.str_assign = 0x003FAE28u,
		.wstr_assign = 0x003FD650u,
		.reload = 0x000CC34Eu,
		.capfin = 0x000AF8C6u,
		.reload_from_ctor = 0x000CC8C8u,
		.imgget = 0x000B9B26u,
		.cap_child_ret = 0x000B49B4u,
		.cap_call = 0x000B49A8u,
		.cap_pos = 0x000B48E2u,
		.cap_list = 0x000B48B6u,
		.fldr_param = 0x000CC676u,
		.paf_free = 0x0044C258u,
		.seg1_g = 0x00011EB4u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "devkit 3.650-3.680",
		.seg0_size = 0x00531234u,
		.seg1_size = 0x00092CC4u,
		.folder_open = 0x000AC454u,
		.folder_close = 0x000A9632u,
		.fl_add = 0x000CE5A8u,
		.fl_del = 0x000CE716u,
		.gate1 = 0x000B2768u,
		.gate2 = 0x000B2BBAu,
		.isfolder = 0x000CC2BCu,
		.iconcapture = 0x000CEA78u,
		.mkfolderjob = 0x001BE6AAu,
		.str_assign = 0x003FB1F8u,
		.wstr_assign = 0x003FDA20u,
		.reload = 0x000CC34Eu,
		.capfin = 0x000AF8C6u,
		.reload_from_ctor = 0x000CC8C8u,
		.imgget = 0x000B9B26u,
		.cap_child_ret = 0x000B49B4u,
		.cap_call = 0x000B49A8u,
		.cap_pos = 0x000B48E2u,
		.cap_list = 0x000B48B6u,
		.fldr_param = 0x000CC676u,
		.paf_free = 0x0044C628u,
		.seg1_g = 0x00011EC4u,
		.seg1_view = 0x00001280u,
	},
	{
		.name = "devkit 3.690-3.730",
		.seg0_size = 0x005312B4u,
		.seg1_size = 0x00092CC4u,
		.folder_open = 0x000AC454u,
		.folder_close = 0x000A9632u,
		.fl_add = 0x000CE5A8u,
		.fl_del = 0x000CE716u,
		.gate1 = 0x000B2768u,
		.gate2 = 0x000B2BBAu,
		.isfolder = 0x000CC2BCu,
		.iconcapture = 0x000CEA78u,
		.mkfolderjob = 0x001BE6AAu,
		.str_assign = 0x003FB21Cu,
		.wstr_assign = 0x003FDA44u,
		.reload = 0x000CC34Eu,
		.capfin = 0x000AF8C6u,
		.reload_from_ctor = 0x000CC8C8u,
		.imgget = 0x000B9B26u,
		.cap_child_ret = 0x000B49B4u,
		.cap_call = 0x000B49A8u,
		.cap_pos = 0x000B48E2u,
		.cap_list = 0x000B48B6u,
		.fldr_param = 0x000CC676u,
		.paf_free = 0x0044C650u,
		.seg1_g = 0x00011EC4u,
		.seg1_view = 0x00001280u,
	},
};

static const shell_profile_t *shell_profile_find(uint32_t seg0_size, uint32_t seg1_size)
{
	for (unsigned i = 0; i < sizeof(shell_profiles) / sizeof(shell_profiles[0]); i++) {
		if (shell_profiles[i].seg0_size == seg0_size &&
		    shell_profiles[i].seg1_size == seg1_size)
			return &shell_profiles[i];
	}
	return 0;
}

#endif
