#include "WeaponIcons.h"

#include "weapon_icons.h"


CWeaponIcon* WeaponIcons = new CWeaponIcon;

void CWeaponIcon::Setup() {
	weap_ak47 = Render->LoadImageFromMemory(weap_icon_ak47, sizeof(weap_icon_ak47), Vector2(33, 12));
	weap_ammobox = Render->LoadImageFromMemory(weap_icon_ammobox, sizeof(weap_icon_ammobox), Vector2(12, 12));
	weap_ammobox_threepack = Render->LoadImageFromMemory(weap_icon_ammobox_threepack, sizeof(weap_icon_ammobox_threepack), Vector2(15, 12));
	weap_armor = Render->LoadImageFromMemory(weap_icon_armor, sizeof(weap_icon_armor), Vector2(9, 12));
	weap_armor_helmet = Render->LoadImageFromMemory(weap_icon_armor_helmet, sizeof(weap_icon_armor_helmet), Vector2(18, 12));
	weap_assaultsuit = Render->LoadImageFromMemory(weap_icon_assaultsuit, sizeof(weap_icon_assaultsuit), Vector2(18, 12));
	weap_aug = Render->LoadImageFromMemory(weap_icon_aug, sizeof(weap_icon_aug), Vector2(28, 12));
	weap_awp = Render->LoadImageFromMemory(weap_icon_awp, sizeof(weap_icon_awp), Vector2(41, 12));
	weap_axe = Render->LoadImageFromMemory(weap_icon_axe, sizeof(weap_icon_axe), Vector2(13, 15));
	weap_bayonet = Render->LoadImageFromMemory(weap_icon_bayonet, sizeof(weap_icon_bayonet), Vector2(34, 12));
	weap_bizon = Render->LoadImageFromMemory(weap_icon_bizon, sizeof(weap_icon_bizon), Vector2(34, 12));
	weap_breachcharge = Render->LoadImageFromMemory(weap_icon_breachcharge, sizeof(weap_icon_breachcharge), Vector2(16, 11));
	weap_breachcharge_projectile = Render->LoadImageFromMemory(weap_icon_breachcharge_projectile, sizeof(weap_icon_breachcharge_projectile), Vector2(11, 10));
	weap_bumpmine = Render->LoadImageFromMemory(weap_icon_bumpmine, sizeof(weap_icon_bumpmine), Vector2(11, 11));
	weap_c4 = Render->LoadImageFromMemory(weap_icon_c4, sizeof(weap_icon_c4), Vector2(7, 12));
	weap_controldrone = Render->LoadImageFromMemory(weap_icon_controldrone, sizeof(weap_icon_controldrone), Vector2(12, 12));
	weap_cz75a = Render->LoadImageFromMemory(weap_icon_cz75a, sizeof(weap_icon_cz75a), Vector2(18, 12));
	weap_deagle = Render->LoadImageFromMemory(weap_icon_deagle, sizeof(weap_icon_deagle), Vector2(19, 12));
	weap_decoy = Render->LoadImageFromMemory(weap_icon_decoy, sizeof(weap_icon_decoy), Vector2(9, 12));
	weap_defuser = Render->LoadImageFromMemory(weap_icon_defuser, sizeof(weap_icon_defuser), Vector2(10, 12));
	weap_diversion = Render->LoadImageFromMemory(weap_icon_diversion, sizeof(weap_icon_diversion), Vector2(9, 12));
	weap_dronegun = Render->LoadImageFromMemory(weap_icon_dronegun, sizeof(weap_icon_dronegun), Vector2(14, 12));
	weap_elite = Render->LoadImageFromMemory(weap_icon_elite, sizeof(weap_icon_elite), Vector2(25, 12));
	weap_famas = Render->LoadImageFromMemory(weap_icon_famas, sizeof(weap_icon_famas), Vector2(29, 12));
	weap_firebomb = Render->LoadImageFromMemory(weap_icon_firebomb, sizeof(weap_icon_firebomb), Vector2(7, 12));
	weap_fists = Render->LoadImageFromMemory(weap_icon_fists, sizeof(weap_icon_fists), Vector2(12, 14));
	weap_fiveseven = Render->LoadImageFromMemory(weap_icon_fiveseven, sizeof(weap_icon_fiveseven), Vector2(14, 12));
	weap_flashbang = Render->LoadImageFromMemory(weap_icon_flashbang, sizeof(weap_icon_flashbang), Vector2(9, 12));
	weap_flashbang_assist = Render->LoadImageFromMemory(weap_icon_flashbang_assist, sizeof(weap_icon_flashbang_assist), Vector2(11, 11));
	weap_frag_grenade = Render->LoadImageFromMemory(weap_icon_frag_grenade, sizeof(weap_icon_frag_grenade), Vector2(7, 12));
	weap_g3sg1 = Render->LoadImageFromMemory(weap_icon_g3sg1, sizeof(weap_icon_g3sg1), Vector2(35, 12));
	weap_galilar = Render->LoadImageFromMemory(weap_icon_galilar, sizeof(weap_icon_galilar), Vector2(34, 12));
	weap_glock = Render->LoadImageFromMemory(weap_icon_glock, sizeof(weap_icon_glock), Vector2(17, 12));
	weap_grenadepack = Render->LoadImageFromMemory(weap_icon_grenadepack, sizeof(weap_icon_grenadepack), Vector2(14, 12));
	weap_grenadepack2 = Render->LoadImageFromMemory(weap_icon_grenadepack2, sizeof(weap_icon_grenadepack2), Vector2(12, 12));
	weap_hammer = Render->LoadImageFromMemory(weap_icon_hammer, sizeof(weap_icon_hammer), Vector2(22, 13));
	weap_healthshot = Render->LoadImageFromMemory(weap_icon_healthshot, sizeof(weap_icon_healthshot), Vector2(15, 11));
	weap_heavy_armor = Render->LoadImageFromMemory(weap_icon_heavy_armor, sizeof(weap_icon_heavy_armor), Vector2(18, 12));
	weap_hegrenade = Render->LoadImageFromMemory(weap_icon_hegrenade, sizeof(weap_icon_hegrenade), Vector2(7, 12));
	weap_helmet = Render->LoadImageFromMemory(weap_icon_helmet, sizeof(weap_icon_helmet), Vector2(7, 12));
	weap_hkp2000 = Render->LoadImageFromMemory(weap_icon_hkp2000, sizeof(weap_icon_hkp2000), Vector2(12, 12));
	weap_incgrenade = Render->LoadImageFromMemory(weap_icon_incgrenade, sizeof(weap_icon_incgrenade), Vector2(4, 12));
	weap_inferno = Render->LoadImageFromMemory(weap_icon_inferno, sizeof(weap_icon_inferno), Vector2(11, 10));
	weap_kevlar = Render->LoadImageFromMemory(weap_icon_kevlar, sizeof(weap_icon_kevlar), Vector2(9, 12));
	weap_knife = Render->LoadImageFromMemory(weap_icon_knife, sizeof(weap_icon_knife), Vector2(29, 12));
	weap_knifegg = Render->LoadImageFromMemory(weap_icon_knifegg, sizeof(weap_icon_knifegg), Vector2(36, 12));
	weap_knife_bowie = Render->LoadImageFromMemory(weap_icon_knife_bowie, sizeof(weap_icon_knife_bowie), Vector2(38, 12));
	weap_knife_butterfly = Render->LoadImageFromMemory(weap_icon_knife_butterfly, sizeof(weap_icon_knife_butterfly), Vector2(30, 12));
	weap_knife_canis = Render->LoadImageFromMemory(weap_icon_knife_canis, sizeof(weap_icon_knife_canis), Vector2(34, 12));
	weap_knife_cord = Render->LoadImageFromMemory(weap_icon_knife_cord, sizeof(weap_icon_knife_cord), Vector2(34, 12));
	weap_knife_css = Render->LoadImageFromMemory(weap_icon_knife_css, sizeof(weap_icon_knife_css), Vector2(34, 12));
	weap_knife_falchion = Render->LoadImageFromMemory(weap_icon_knife_falchion, sizeof(weap_icon_knife_falchion), Vector2(33, 12));
	weap_knife_flip = Render->LoadImageFromMemory(weap_icon_knife_flip, sizeof(weap_icon_knife_flip), Vector2(30, 12));
	weap_knife_gut = Render->LoadImageFromMemory(weap_icon_knife_gut, sizeof(weap_icon_knife_gut), Vector2(27, 12));
	weap_knife_gypsy_jackknife = Render->LoadImageFromMemory(weap_icon_knife_gypsy_jackknife, sizeof(weap_icon_knife_gypsy_jackknife), Vector2(34, 12));
	weap_knife_karambit = Render->LoadImageFromMemory(weap_icon_knife_karambit, sizeof(weap_icon_knife_karambit), Vector2(25, 12));
	weap_knife_m9_bayonet = Render->LoadImageFromMemory(weap_icon_knife_m9_bayonet, sizeof(weap_icon_knife_m9_bayonet), Vector2(33, 12));
	weap_knife_outdoor = Render->LoadImageFromMemory(weap_icon_knife_outdoor, sizeof(weap_icon_knife_outdoor), Vector2(34, 12));
	weap_knife_push = Render->LoadImageFromMemory(weap_icon_knife_push, sizeof(weap_icon_knife_push), Vector2(31, 12));
	weap_knife_skeleton = Render->LoadImageFromMemory(weap_icon_knife_skeleton, sizeof(weap_icon_knife_skeleton), Vector2(34, 12));
	weap_knife_stiletto = Render->LoadImageFromMemory(weap_icon_knife_stiletto, sizeof(weap_icon_knife_stiletto), Vector2(34, 11));
	weap_knife_survival_bowie = Render->LoadImageFromMemory(weap_icon_knife_survival_bowie, sizeof(weap_icon_knife_survival_bowie), Vector2(23, 12));
	weap_knife_t = Render->LoadImageFromMemory(weap_icon_knife_t, sizeof(weap_icon_knife_t), Vector2(30, 12));
	weap_knife_tactical = Render->LoadImageFromMemory(weap_icon_knife_tactical, sizeof(weap_icon_knife_tactical), Vector2(36, 12));
	weap_knife_ursus = Render->LoadImageFromMemory(weap_icon_knife_ursus, sizeof(weap_icon_knife_ursus), Vector2(34, 12));
	weap_knife_widowmaker = Render->LoadImageFromMemory(weap_icon_knife_widowmaker, sizeof(weap_icon_knife_widowmaker), Vector2(34, 12));
	weap_m249 = Render->LoadImageFromMemory(weap_icon_m249, sizeof(weap_icon_m249), Vector2(36, 12));
	weap_m4a1 = Render->LoadImageFromMemory(weap_icon_m4a1, sizeof(weap_icon_m4a1), Vector2(29, 12));
	weap_m4a1_silencer = Render->LoadImageFromMemory(weap_icon_m4a1_silencer, sizeof(weap_icon_m4a1_silencer), Vector2(36, 12));
	weap_m4a1_silencer_off = Render->LoadImageFromMemory(weap_icon_m4a1_silencer_off, sizeof(weap_icon_m4a1_silencer_off), Vector2(30, 12));
	weap_mac10 = Render->LoadImageFromMemory(weap_icon_mac10, sizeof(weap_icon_mac10), Vector2(16, 12));
	weap_mag7 = Render->LoadImageFromMemory(weap_icon_mag7, sizeof(weap_icon_mag7), Vector2(25, 12));
	weap_molotov = Render->LoadImageFromMemory(weap_icon_molotov, sizeof(weap_icon_molotov), Vector2(7, 12));
	weap_mp5sd = Render->LoadImageFromMemory(weap_icon_mp5sd, sizeof(weap_icon_mp5sd), Vector2(35, 14));
	weap_mp7 = Render->LoadImageFromMemory(weap_icon_mp7, sizeof(weap_icon_mp7), Vector2(18, 12));
	weap_mp9 = Render->LoadImageFromMemory(weap_icon_mp9, sizeof(weap_icon_mp9), Vector2(27, 12));
	weap_negev = Render->LoadImageFromMemory(weap_icon_negev, sizeof(weap_icon_negev), Vector2(30, 12));
	weap_nova = Render->LoadImageFromMemory(weap_icon_nova, sizeof(weap_icon_nova), Vector2(38, 12));
	weap_p2000 = Render->LoadImageFromMemory(weap_icon_p2000, sizeof(weap_icon_p2000), Vector2(14, 12));
	weap_p250 = Render->LoadImageFromMemory(weap_icon_p250, sizeof(weap_icon_p250), Vector2(14, 12));
	weap_p90 = Render->LoadImageFromMemory(weap_icon_p90, sizeof(weap_icon_p90), Vector2(25, 12));
	weap_planted_c4 = Render->LoadImageFromMemory(weap_icon_planted_c4, sizeof(weap_icon_planted_c4), Vector2(11, 10));
	weap_planted_c4_survival = Render->LoadImageFromMemory(weap_icon_planted_c4_survival, sizeof(weap_icon_planted_c4_survival), Vector2(11, 10));
	weap_prop_exploding_barrel = Render->LoadImageFromMemory(weap_icon_prop_exploding_barrel, sizeof(weap_icon_prop_exploding_barrel), Vector2(11, 10));
	weap_radarjammer = Render->LoadImageFromMemory(weap_icon_radarjammer, sizeof(weap_icon_radarjammer), Vector2(12, 12));
	weap_revolver = Render->LoadImageFromMemory(weap_icon_revolver, sizeof(weap_icon_revolver), Vector2(19, 12));
	weap_sawedoff = Render->LoadImageFromMemory(weap_icon_sawedoff, sizeof(weap_icon_sawedoff), Vector2(32, 12));
	weap_scar20 = Render->LoadImageFromMemory(weap_icon_scar20, sizeof(weap_icon_scar20), Vector2(37, 12));
	weap_sg556 = Render->LoadImageFromMemory(weap_icon_sg556, sizeof(weap_icon_sg556), Vector2(34, 12));
	weap_shield = Render->LoadImageFromMemory(weap_icon_shield, sizeof(weap_icon_shield), Vector2(7, 12));
	weap_smokegrenade = Render->LoadImageFromMemory(weap_icon_smokegrenade, sizeof(weap_icon_smokegrenade), Vector2(4, 12));
	weap_snowball = Render->LoadImageFromMemory(weap_icon_snowball, sizeof(weap_icon_snowball), Vector2(10, 12));
	weap_spanner = Render->LoadImageFromMemory(weap_icon_spanner, sizeof(weap_icon_spanner), Vector2(24, 11));
	weap_ssg08 = Render->LoadImageFromMemory(weap_icon_ssg08, sizeof(weap_icon_ssg08), Vector2(37, 12));
	weap_stomp_damage = Render->LoadImageFromMemory(weap_icon_stomp_damage, sizeof(weap_icon_stomp_damage), Vector2(14, 9));
	weap_tablet = Render->LoadImageFromMemory(weap_icon_tablet, sizeof(weap_icon_tablet), Vector2(17, 12));
	weap_tagrenade = Render->LoadImageFromMemory(weap_icon_tagrenade, sizeof(weap_icon_tagrenade), Vector2(7, 12));
	weap_taser = Render->LoadImageFromMemory(weap_icon_taser, sizeof(weap_icon_taser), Vector2(15, 12));
	weap_tec9 = Render->LoadImageFromMemory(weap_icon_tec9, sizeof(weap_icon_tec9), Vector2(19, 12));
	weap_ump45 = Render->LoadImageFromMemory(weap_icon_ump45, sizeof(weap_icon_ump45), Vector2(31, 12));
	weap_usp_silencer = Render->LoadImageFromMemory(weap_icon_usp_silencer, sizeof(weap_icon_usp_silencer), Vector2(26, 12));
	weap_usp_silencer_off = Render->LoadImageFromMemory(weap_icon_usp_silencer_off, sizeof(weap_icon_usp_silencer_off), Vector2(15, 12));
	weap_xm1014 = Render->LoadImageFromMemory(weap_icon_xm1014, sizeof(weap_icon_xm1014), Vector2(37, 12));
	weap_zone_repulsor = Render->LoadImageFromMemory(weap_icon_zone_repulsor, sizeof(weap_icon_zone_repulsor), Vector2(12, 12));
}

DXImage& CWeaponIcon::GetIcon(int weapon_id) {
	switch (weapon_id)
	{
	case Deagle:
		return weap_deagle;
	case Elite:
		return weap_elite;
	case Fiveseven:
		return weap_fiveseven;
	case Glock:
		return weap_glock;
	case Ak47:
		return weap_ak47;
	case Aug:
		return weap_aug;
	case Awp:
		return weap_awp;
	case Famas:
		return weap_famas;
	case G3SG1:
		return weap_g3sg1;
	case GalilAr:
		return weap_galilar;
	case M249:
		return weap_m249;
	case M4A1:
		return weap_m4a1;
	case Mac10:
		return weap_mac10;
	case P90:
		return weap_p90;
	case ZoneRepulsor:
		return weap_zone_repulsor;
	case Mp5sd:
		return weap_mp5sd;
	case Ump45:
		return weap_ump45;
	case Xm1014:
		return weap_xm1014;
	case Bizon:
		return weap_bizon;
	case Mag7:
		return weap_mag7;
	case Negev:
		return weap_negev;
	case Sawedoff:
		return weap_sawedoff;
	case Tec9:
		return weap_tec9;
	case Taser:
		return weap_taser;
	case Hkp2000:
		return weap_hkp2000;
	case Mp7:
		return weap_mp7;
	case Mp9:
		return weap_mp9;
	case Nova:
		return weap_nova;
	case P250:
		return weap_p250;
	case Shield:
		return weap_shield;
	case Scar20:
		return weap_scar20;
	case Sg553:
		return weap_sg556;
	case Ssg08:
		return weap_ssg08;
	case GoldenKnife:
		return weap_knifegg;
	case Knife:
		return weap_knife;
	case Flashbang:
		return weap_flashbang;
	case HeGrenade:
		return weap_hegrenade;
	case C4:
		return weap_c4;
	case Healthshot:
		return weap_healthshot;
	case KnifeT:
		return weap_knife_t;
	case M4a1_s:
		return weap_m4a1_silencer;
	case Usp_s:
		return weap_usp_silencer;
	case Cz75a:
		return weap_cz75a;
	case Revolver:
		return weap_revolver;
	case TaGrenade:
		return weap_tagrenade;
	case Axe:
		return weap_axe;
	case Hammer:
		return weap_hammer;
	case Spanner:
		return weap_spanner;
	case GhostKnife:
		return weap_knife_flip;
	case Firebomb:
		return weap_firebomb;
	case Diversion:
		return weap_diversion;
	case FragGrenade:
		return weap_frag_grenade;
	case Snowball:
		return weap_snowball;
	case BumpMine:
		return weap_bumpmine;
	case Bayonet:
		return weap_bayonet;
	case ClassicKnife:
		return weap_knife_css;
	case Flip:
		return weap_knife_flip;
	case Gut:
		return weap_knife_gut;
	case Karambit:
		return weap_knife_karambit;
	case M9Bayonet:
		return weap_knife_m9_bayonet;
	case Huntsman:
		return weap_knife_tactical;
	case Falchion:
		return weap_knife_falchion;
	case Bowie:
		return weap_knife_survival_bowie;
	case Butterfly:
		return weap_knife_butterfly;
	case Daggers:
		return weap_knife_push;
	case Paracord:
		return weap_knife_cord;
	case SurvivalKnife:
		return weap_knife_canis;
	case Ursus:
		return weap_knife_ursus;
	case Navaja:
		return weap_knife_gypsy_jackknife;
	case NomadKnife:
		return weap_knife_outdoor;
	case Stiletto:
		return weap_knife_stiletto;
	case Talon:
		return weap_knife_widowmaker;
	case SkeletonKnife:
		return weap_knife_skeleton;
	case Molotov:
		return weap_molotov;
	case IncGrenade:
		return weap_incgrenade;
	case SmokeGrenade:
		return weap_smokegrenade;
	default:
		break;
	}

	static DXImage nullimage;
	return nullimage;
}