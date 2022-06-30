//
//  fileio_party.cpp
//  BoE
//
//  Created by Celtic Minstrel on 15-01-24.
//
//

#include "fileio.hpp"

#include <fstream>
#include <boost/filesystem/operations.hpp>

#include "strdlog.hpp"
#include "gzstream.h"

#include "universe.hpp"
#include "gfxsheets.hpp"

#include "porting.hpp"
#include "tarball.hpp"

extern bool mac_is_intel;
extern fs::path progDir, tempDir;
extern cCustomGraphics spec_scen_g;

// Load saved games
static bool load_party_v1(fs::path file_to_load, cUniverse& univ, bool town_restore, bool in_scen, bool maps_there, bool must_port);
static bool load_party_v2(fs::path file_to_load, cUniverse& univ);

bool load_party(fs::path file_to_load, cUniverse& univ){
	bool town_restore = false;
	bool maps_there = false;
	bool in_scen = false;
	enum {old_mac, old_win, new_oboe, unknown} format;
	typedef unsigned short ushort;
	
	long len;
	short n;
	struct {ushort a; ushort b; ushort c; ushort d;} flags;
	
	// TODO: Putting these flags in hex would make some things a bit clearer
	static const unsigned short mac_flags[3][2] = {
		{5790,1342}, // slot 0 ... 5790 - out,         1342 - town
		{100,200},   // slot 1 ... 100  - in scenario, 200  - not in
		{3422,5567}  // slot 2 ... 3422 - no maps,     5567 - maps
	};
	static const unsigned short win_flags[3][2] = {
		{40470,15877}, // slot 0 ... 40470 - out,         15877 - town
		{25600,51200}, // slot 1 ... 25600 - in scenario, 51200 - not in
		{24077,48917}  // slot 2 ... 24077 - no maps,     48917 - maps
	};
	// but if the first flag is 0x0B0E, we have a new-format save
	// the three flags still follow that.
	FILE* file_id = fopen(file_to_load.string().c_str(), "rb");
	if(file_id == nullptr) {
		showError("Loading Blades of Exile save file failed.");
		return false;
	}
	
	len = sizeof(flags); // 10
	
	n = fread(&flags, len, 1, file_id);
	if(n < 1) {
		fclose(file_id);
		showError("This is not a Blades of Exile save file.");
		return false;
	}
	
	if(mac_is_intel && flags.a == 0x8B1F){ // Gzip header (new format)
		format = new_oboe;
	}else if(!mac_is_intel && flags.a == 0x1F8B){ // Gzip header (new format)
		format = new_oboe;
	}else if(flags.a == mac_flags[0][0] || flags.a == mac_flags[0][1]){ // old format
		if(mac_is_intel){ // it's actually a windows save
			flip_short((short*)&flags.a);
			flip_short((short*)&flags.b);
			flip_short((short*)&flags.c);
			format = old_win;
			if(flags.a == win_flags[0][1]) town_restore = true;
			else if(flags.a != win_flags[0][0]) format = unknown;
			if(flags.b == win_flags[1][0]) in_scen = true;
			else if(flags.b != win_flags[1][1]) format = unknown;
			if(flags.c == win_flags[2][1]) maps_there = true;
			else if(flags.c != win_flags[2][0]) format = unknown;
		}else{ // mac save
			format = old_mac;
			if(flags.a == mac_flags[0][1]) town_restore = true;
			else if(flags.a != mac_flags[0][0]) format = unknown;
			if(flags.b == mac_flags[1][0]) in_scen = true;
			else if(flags.b != mac_flags[1][1]) format = unknown;
			if(flags.c == mac_flags[2][1]) maps_there = true;
			else if(flags.c != mac_flags[2][0]) format = unknown;
		}
	}else if(flags.a == win_flags[0][0] || flags.a == win_flags[0][1]){ // old format
		if(mac_is_intel){ // it's actually a macintosh save
			flip_short((short*)&flags.a);
			flip_short((short*)&flags.b);
			flip_short((short*)&flags.c);
			format = old_mac;
			if(flags.a == mac_flags[0][1]) town_restore = true;
			else if(flags.a != mac_flags[0][0]) format = unknown;
			if(flags.b == mac_flags[1][0]) in_scen = true;
			else if(flags.b != mac_flags[1][1]) format = unknown;
			if(flags.c == mac_flags[2][1]) maps_there = true;
			else if(flags.c != mac_flags[2][0]) format = unknown;
		}else{ // win save
			format = old_win;
			if(flags.a == win_flags[0][1]) town_restore = true;
			else if(flags.a != win_flags[0][0]) format = unknown;
			if(flags.b == win_flags[1][0]) in_scen = true;
			else if(flags.b != win_flags[1][1]) format = unknown;
			if(flags.c == win_flags[2][1]) maps_there = true;
			else if(flags.c != win_flags[2][0]) format = unknown;
		}
	}else format = unknown;
	
	fclose(file_id);
	switch(format){
		case old_mac:
			return load_party_v1(file_to_load, univ, town_restore, in_scen, maps_there, mac_is_intel);
		case old_win:
			return load_party_v1(file_to_load, univ, town_restore, in_scen, maps_there, !mac_is_intel);
		case new_oboe:
			return load_party_v2(file_to_load, univ);
		case unknown:
			showError("This is not a Blades of Exile save file.");
			return false;
	}
	
	return true;
}

bool load_party_v1(fs::path file_to_load, cUniverse& real_univ, bool town_restore, bool in_scen, bool maps_there, bool must_port){
	std::ifstream fin(file_to_load.string().c_str(), std::ios_base::binary);
	fin.seekg(3*sizeof(short),std::ios_base::beg); // skip the header, which is 6 bytes in the old format
	
	legacy::party_record_type store_party;
	legacy::setup_save_type store_setup;
	legacy::pc_record_type store_pc[6];
	legacy::out_info_type store_out_info;
	legacy::current_town_type store_c_town;
	legacy::big_tr_type t_d;
	legacy::town_item_list t_i;
	legacy::stored_items_list_type stored_items[3];
	legacy::stored_town_maps_type town_maps;
	legacy::stored_outdoor_maps_type o_maps;
	unsigned char misc_i[64][64], sfx[64][64];
	char *party_ptr;
	char *pc_ptr;
	long len,store_len,count;
	
	// LOAD PARTY
	len = (long) sizeof(legacy::party_record_type); // should be 46398
	store_len = len;
	fin.read((char*)&store_party, len);
	if(must_port) port_party(&store_party);
	party_ptr = (char*) &store_party;
	for(count = 0; count < store_len; count++)
		party_ptr[count] ^= 0x5C;
	
	// LOAD SETUP
	len = (long) sizeof(legacy::setup_save_type);
	fin.read((char*)&store_setup, len);
	
	// LOAD PCS
	store_len = (long) sizeof(legacy::pc_record_type);
	for(int i = 0; i < 6; i++) {
		len = store_len;
		fin.read((char*)&store_pc[i], len);
		if(must_port) port_pc(&store_pc[i]);
		pc_ptr = (char*) &store_pc[i];
		for(count = 0; count < store_len; count++)
			pc_ptr[count] ^= 0x6B;
	}
	
	if(in_scen) {
		
		// LOAD OUTDOOR MAP
		len = (long) sizeof(legacy::out_info_type);
		fin.read((char*)&store_out_info, len);
		
		// LOAD TOWN
		if(town_restore) {
			len = (long) sizeof(legacy::current_town_type);
			fin.read((char*)&store_c_town, len);
			if(must_port) port_c_town(&store_c_town);
			
			len = (long) sizeof(legacy::big_tr_type);
			fin.read((char*)&t_d, len);
			if(must_port) port_t_d(&t_d);
			
			len = (long) sizeof(legacy::town_item_list);
			fin.read((char*)&t_i, len);
		}
		
		// LOAD STORED ITEMS
		for(int i = 0; i < 3; i++) {
			len = (long) sizeof(legacy::stored_items_list_type);
			fin.read((char*)&stored_items[i], len);
		}
		
		// LOAD SAVED MAPS
		if(maps_there) {
			len = (long) sizeof(legacy::stored_town_maps_type);
			fin.read((char*)&town_maps, len);
			
			len = (long) sizeof(legacy::stored_outdoor_maps_type);
			fin.read((char*)&o_maps, len);
		}
		
		// LOAD SFX & MISC_I
		len = (long) (64 * 64);
		fin.read((char*)sfx, len);
		
		fin.read((char*)misc_i, len);
		
	} // end if_scen
	
	fin.close();
	
	cUniverse univ;
	
	if(in_scen){
		fs::path path = locate_scenario(store_party.scen_name);
		if(path.empty()) {
			std::ostringstream msg;
			msg << "The scenario that this party was in (\"" << store_party.scen_name;
			msg << "\") could not be found. Most likely, it is not installed in the Scenarios directory.";
			showError(msg.str());
			return false;
		}
		
		if(!load_scenario(path, univ.scenario))
			return false;
		univ.file = path;
	}else{
		univ.party.scen_name = "";
	}
	
	univ.party.import_legacy(store_party, univ);
	univ.party.import_legacy(store_setup);
	univ.party.import_legacy(store_pc);
	if(in_scen){
		univ.out.import_legacy(store_out_info);
		if(town_restore){
			univ.town.import_legacy(store_c_town);
			univ.town.import_legacy(t_d);
			univ.town.import_legacy(t_i);
		}
		for(int i = 0; i < 3; i++)
			univ.party.import_legacy(stored_items[i],i);
		univ.import_legacy(town_maps);
		univ.import_legacy(o_maps);
		univ.town.import_legacy(sfx, misc_i);
		if(town_restore) // Check items in crates/barrels
			for(int i = 0; i < univ.town->max_dim; i++) {
				for(int j = 0; j < univ.town->max_dim; j++) {
					if(univ.town.is_barrel(i,j) || univ.town.is_crate(i,j)) {
						for(cItem item : univ.town.items) {
							if(item.item_loc == loc(i,j) && item.contained)
								item.held = true;
						}
					}
				}
			}
		else univ.party.town_num = 200;
	}
	
	real_univ = std::move(univ);
	return true;
}

extern fs::path scenDir;
bool load_party_v2(fs::path file_to_load, cUniverse& real_univ){
	igzstream zin(file_to_load.string().c_str());
	tarball partyIn;
	partyIn.readFrom(zin);
	zin.close();
	
	cUniverse univ;
	
	{ // Load main party data first
		std::istream& fin = partyIn.getFile("save/party.txt");
		if(!fin) {
			showError("Loading Blades of Exile save file failed.");
			return false;
		}
		univ.party.readFrom(fin);
	}
	
	{ // Then the "setup" array
		std::istream& fin = partyIn.getFile("save/setup.dat");
		if(!fin) {
			showError("Loading Blades of Exile save file failed.");
			return false;
		}
		uint16_t magic;
		fin.read((char*)&magic, 2);
		fin.read((char*)&univ.party.setup, sizeof(univ.party.setup));
		if(magic == 0x0E0B) // should be 0x0B0E!
			for(auto& i : univ.party.setup)
				for(auto& j : i)
					for(auto& k : j)
						flip_short(reinterpret_cast<int16_t*>(&k));
	}
	
	// Next load the PCs
	for(int i = 0; i < 6; i++) {
		static char fname[] = "save/pc1.txt";
		fname[7] = i + '1';
		std::istream& fin = partyIn.getFile(fname);
		if(!fin) {
			showError("Loading Blades of Exile save file failed.");
			return false;
		}
		univ.party[i].readFrom(fin);
		// Do this to make sure the PC's internal party reference is correct
		univ.party[i].join_party(univ.party);
	}
	
	// Including stored PCs
	if(partyIn.hasFile("save/stored_pcs.txt")) {
		std::istream& fin = partyIn.getFile("save/stored_pcs.txt");
		long next_uid;
		while(fin >> next_uid) {
			std::string fname = "save/pc~" + std::to_string(next_uid) + ".txt";
			cPlayer* stored_pc = new cPlayer(univ.party);
			stored_pc->readFrom(partyIn.getFile(fname));
			univ.stored_pcs[next_uid] = stored_pc->leave_party();
		}
	}
	
	if(!univ.party.scen_name.empty()) {
		fs::path path = locate_scenario(univ.party.scen_name);
		if(path.empty()) {
			std::ostringstream msg;
			msg << "The scenario that this party was in (\"" << univ.party.scen_name;
			msg << "\") could not be found. Most likely, it is not installed in the Scenarios directory.";
			showError(msg.str());
			return false;
		}
		
		if(!load_scenario(path, univ.scenario))
			return false;

		if(partyIn.hasFile("save/scenario.txt")) {
			// Load scenario data
			std::istream& fin = partyIn.getFile("save/scenario.txt");
			if(!fin) {
				showError("Loading Blades of Exile save file failed.");
				return false;
			}
			univ.scenario.readFrom(fin);
		}
		
		if(partyIn.hasFile("save/town.txt")) {
			// Load town data
			std::istream& fin = partyIn.getFile("save/town.txt");
			if(!fin) {
				showError("Loading Blades of Exile save file failed.");
				return false;
			}
			univ.town.readFrom(fin);
		} else univ.party.town_num = 200;

		if (partyIn.hasFile("save/townmaps.dat")) {
			// Read town maps
			std::istream& fin2 = partyIn.getFile("save/townmaps.dat");
			for(int i = 0; i < univ.scenario.towns.size(); i++)
				for(int j = 0; j < univ.scenario.towns[i]->max_dim; j++)
					fin2 >> univ.scenario.towns[i]->maps[j];
		}
		// Load outdoors data
		std::istream& fin = partyIn.getFile("save/out.txt");
		if(!fin) {
			showError("Loading Blades of Exile save file failed.");
			return false;
		}
		univ.out.readFrom(fin);
		
		// Read outdoor maps
		std::istream& fin2 = partyIn.getFile("save/outmaps.dat");
		for(int i = 0; i < univ.scenario.outdoors.height(); i++)
			for(int j = 0; j < 48; j++)
				for(int k = 0; k < univ.scenario.outdoors.width(); k++)
					fin2 >> univ.scenario.outdoors[k][i]->maps[j];
	} else univ.party.scen_name = "";
	
	if(partyIn.hasFile("save/export.png")) {
		std::istream& fin = partyIn.getFile("save/export.png");
		sf::Image party_sheet;
		StdInputStream stream(fin);
		if(party_sheet.loadFromStream(stream)) {
			sf::Texture sheet;
			sheet.create(party_sheet.getSize().x, party_sheet.getSize().y);
			sheet.update(party_sheet);
			spec_scen_g.party_sheet.reset(new sf::Texture(sheet));
		} else showWarning("There was an error loading the party custom graphics.");
	}
	
	real_univ = std::move(univ);
	return true;
}

//mode;  // 0 - normal  1 - save as
bool save_party(fs::path dest_file, const cUniverse& univ) {
	// Make sure it has the proper file extension
	std::string fname = dest_file.filename().string();
	size_t dot = fname.find_last_of('.');
	if(dot == std::string::npos || fname.substr(dot) != ".exg")
		fname += ".exg";
	dest_file = dest_file.parent_path()/fname;
	
	tarball partyOut;
	
	// First, write the main party data
	univ.party.writeTo(partyOut.newFile("save/party.txt"));
	univ.scenario.writeTo(partyOut.newFile("save/scenario.txt"));
	{
		std::ostream& fout = partyOut.newFile("save/setup.dat");
		static uint16_t magic = 0x0B0E;
		fout.write((char*)&magic, 2);
		fout.write((char*)&univ.party.setup, sizeof(univ.party.setup));
	}
	
	// Then write the data for each of the party members
	for(int i = 0; i < 6; i++) {
		static char fname[] = "save/pc1.txt";
		fname[7] = i + '1';
		univ.party[i].writeTo(partyOut.newFile(fname));
	}
	
	// And stored PCs
	if(univ.stored_pcs.size()) {
		std::ostream& fout = partyOut.newFile("save/stored_pcs.txt");
		for(auto p : univ.stored_pcs) {
			std::string fname = "save/pc~" + std::to_string(p.first) + ".txt";
			p.second->writeTo(partyOut.newFile(fname));
			fout << p.first << '\n';
		}
	}
	
	if(!univ.party.scen_name.empty()) {
		if(univ.party.town_num < 200) {
			// Write the current town data
			univ.town.writeTo(partyOut.newFile("save/town.txt"));
		}
		
		{	
			// Write the town map data
			std::ostream& fout = partyOut.newFile("save/townmaps.dat");
			for(int i = 0; i < univ.scenario.towns.size(); i++)
				for(int j = 0; j < univ.scenario.towns[i]->max_dim; j++)
					fout << univ.scenario.towns[i]->maps[j] << '\n';
		}
		
		// Write the current outdoors data
		univ.out.writeTo(partyOut.newFile("save/out.txt"));
		
		// Write the outdoors map data
		std::ostream& fout = partyOut.newFile("save/outmaps.dat");
		for(int i = 0; i < univ.scenario.outdoors.height(); i++) {
			for(int j = 0; j < 48; j++) {
				for(int k = 0; k < univ.scenario.outdoors.width(); k++)
					fout << univ.scenario.outdoors[k][i]->maps[j] << ' ';
				fout << '\n';
			}
			fout << '\n';
		}
	}
	
	if(spec_scen_g.party_sheet) {
		sf::Image party_pics = spec_scen_g.party_sheet->copyToImage();
		party_pics.flipVertically();
		fs::path tempPath = tempDir/"temp.png";
		party_pics.saveToFile(tempPath.string());
		std::ostream& pic_out = partyOut.newFile("save/export.png");
		std::ifstream fin(tempPath.string().c_str(), std::ios::binary);
		pic_out << fin.rdbuf();
		fin.close();
	}
	
	// Write out the compressed data
	ogzstream zout(dest_file.string().c_str());
	partyOut.writeTo(zout);
	zout.close();
	return true;
}

