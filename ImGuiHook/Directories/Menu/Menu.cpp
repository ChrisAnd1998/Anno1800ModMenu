#include <string>
#include <fstream>
#include <iostream>
#include <sstream> //std::stringstream
#include <direct.h>
#include <wtypes.h>
#include <vector>
#include "Menu.h"
#include "../ImGui/imgui.h"
#include "TextEditor.h"
#include "imfilebrowser.h"

#define GetCurrentDir _getcwd

typedef int(__stdcall* PY_ISINITIALIZED)();
typedef int(__stdcall* PYRUN_SIMPLESTRING)(const char* str);
typedef int(__stdcall* PYRUN_SIMPLESTRINGFLAGS)(const char* str, DWORD* flags);
typedef DWORD* (__stdcall* PYGILSTATE_ENSURE)();
typedef void(__stdcall* PYGILSTATE_RELEASE)(DWORD* gstate);

HMODULE hModule = GetModuleHandle("python35.dll");
PY_ISINITIALIZED Py_IsInitialized = (PY_ISINITIALIZED)GetProcAddress(hModule, "Py_IsInitialized");
PYGILSTATE_ENSURE PyGILState_Ensure = (PYGILSTATE_ENSURE)GetProcAddress(hModule, "PyGILState_Ensure");
PYGILSTATE_RELEASE PyGILState_Release = (PYGILSTATE_RELEASE)GetProcAddress(hModule, "PyGILState_Release");
PYRUN_SIMPLESTRING PyRun_SimpleString = (PYRUN_SIMPLESTRING)GetProcAddress(hModule, "PyRun_SimpleString");
PYRUN_SIMPLESTRINGFLAGS PyRun_SimpleStringFlags = (PYRUN_SIMPLESTRINGFLAGS)GetProcAddress(hModule, "PyRun_SimpleStringFlags");

using namespace ImGui;

TextEditor editor;

// create a file browser instance
ImGui::FileBrowser fileDialog;

bool savefile = false;

static const char* fileToEdit;

std::string currentFile = "";

void fileEdit(std::string file) {
	fileToEdit = NULL;
	fileToEdit = (char*)file.c_str();
	//currentFile = file;

	//	static const char* fileToEdit = "test.cpp";
		std::ifstream inFile;
		inFile.open(fileToEdit); //open the input file

		std::stringstream strStream;
		strStream << inFile.rdbuf(); //read the file
		std::string str = strStream.str(); //str holds the content of the file

		editor.SetText(str);
}

std::string curDir(std::string file)
{
	char buffer[MAX_PATH];
	GetCurrentDir(buffer, sizeof(buffer));
	std::string::size_type pos = std::string(buffer).find_last_of("\\/") - 1;
	return std::string(buffer) + "\\" + file;
}

void sendCommand(std::string cmd)
{
	std::ofstream outfile(curDir("script.lua"));
	outfile << cmd + "\r\n" << std::endl;
	outfile.close();

	std::string scriptPath = curDir("script.lua");
	std::size_t found = scriptPath.find_first_of("\\");
	while (found != std::string::npos)
	{
		scriptPath[found] = '/';
		found = scriptPath.find_first_of("\\", found + 1);
	}

	if (Py_IsInitialized != 0)
	{
		DWORD* gstate = PyGILState_Ensure();
		std::string command = "console.startScript(\"" + scriptPath + "\")";
		PyRun_SimpleStringFlags(command.c_str(), NULL);
		PyGILState_Release(gstate);
	}
}

bool showModEditor = false;

static const char* items[] = { "18-Pounder Long Gun", "1801 Nights", "50 Shades of Gravy", "A Bottle of Champagne", "A Child's Letter", "A Dithyrambic Note From the Queen", "A Letter to Kahina", "A Letter to Kahina", "A Letter to Kahina", "A Letter to Kahina", "A Mighty Martial Law Declaration", "A Nice Cup Of Darjeeling", "A Note From the Queen", "A Remedy", "A Shimmering Stone", "A Study For Men", "A Vindication of Women's Rights", "Aaden Issack, World-Famous Enbesan Chef", "Abbé", "Absinthe", "Abus Gun", "Actor", "Adjustable Mannequin", "Admiral Haynes, Beholder of Far Horizons.", "Admiral Nadasky's Sabre", "Advanced Agricultural Textbook", "Advanced Fertiliser", "Advanced Industrial Toxin Filter", "Advanced Pyrphorian Calciner", "Advanced Sluice Gate", "Advanced Soil Sampler", "Advanced Trash Filter", "Advanced Umbilical Cable", "Advanced Weapons", "Aesop, the Fabled Storyteller", "Affiliated Anarchist", "Agriculturalist", "Airmail Stamp", "Airship Cargo Loader", "Airship Engine Mechanic", "Airship Logistician", "Airship Munitions Loader", "Airship Technician", "Akdamot Port Registry", "Albino Alligator", "Albino Fennec", "Albino Gorilla", "Alcohol Crate", "Alexander Hancock", "Algologist", "Alligator Gar", "Alonso's Blunted Bone Saw", "Alonso's Hand Drill", "Alonso's Tranquilizers", "Alpaca Wool", "Alpaca", "Altar of Uppsidaisi", "AlterN8 Current Generator", "Aluminium Profiles", "Amadi Ilga, Ketema's Chief Civil Engineer", "Amazing Fashion Designer", "Amelia Coleman, Expert Aviator", "Ammonia", "Ammonites", "Amnesty on Religious Symbology", "An Anarchist: A Self-Portrait", "An Essay on Birth Control", "An Old Medallion", "Anarchist's Bomb Pack", "Ancient Manifest", "Ancient Tabot of Waha Desher", "Angel Wings Lotus", "Angela Meg Iver, The Polyvalent", "Angelic Anthrax Vaccine", "Angler", "Angler", "Animal Figure", "Animal Nutritionist", "Anna Union, Engineer of Extravagant Steamers", "Anne Kenyatta, Special Needs Teacher", "Anne's Gramophone", "Anne's Message in A Bottle", "Anthony Anchovy, Salvager of Sunken Sumps", "Anti-Bourgeois Cocktail", "Anti-Bourgeois Gas Pipe", "Anti-Bourgeois Midnight Infusion", "Anti-Clerical Lecturer", "Antique Novel", "Antiseptics", "Antler Iggaak", "Apatosaurus", "Aquafarm Fish Eggs (Limited Use)", "Aquafarm Fish Eggs (Persistent)", "Arabesque Trinkets", "Arboriculturist", "Arborist", "Arch-Archbishop Archibald", "Archbishop Stipwick's Stiff-Lipped Curfew", "Arctic Fox", "Arctic Gas Expert", "Arctic Gas", "Arctic Science Reporter", "Arctic Wolf", "Arek Lisowski, the Architect", "Aristelia Bataille, of the Novelty Store", "Arksum Obelisk", "Armagnac Bottles", "Arnarunn the Seamless", "Arsonist", "Art and Craft Festival Poster", "Artful Inuit Hunter", "Arthur Ramshackle's Grand Reservoir Plans", "Artillerywoman", "Artur's Magnifying Glass", "Artur's Portfolio", "Aspirin Machine", "AssassiN8r", "Assembly Hall of Blissful Joy and Tireless Labour", "Assembly Line", "Associate Professor of Faculty", "Astute Dog-Breeder", "Atlantean Tablet", "Atlantean Wall", "Atole", "Atole", "Atole", "Audrey the Marine Archaeologist", "Augustan Forum", "Aureus Coins", "Authentic Ark of the Covenant", "Automatic Souvenir Kiosk", "Auxetophone", "Award-Winning Brewer", "Bag of Money", "Baiji", "Bait Specialist", "Baker", "Balance Plough", "Ball and Chain", "Ballast Cable", "Ballasted Diving Bell", "Balloon Fish", "Bank Holiday Decree", "Banker", "Banner of La Corona", "Banquet Chef", "Bar Shots", "Barbarossa's Bones", "Bartender", "Basic Agricultural Textbook", "Basic Fertiliser", "Bat-Eared Fennec", "Bauxite", "Bean Sifter", "Bear Fur", "Beard of the Year Award", "Bearded Vulture", "Beatific Bishopess", "Bechamel Converter", "Beef", "Beer", "Beeswax", "Bekonin, Spirit of Liberty", "Belinda San Pedro, Head of Arcade Acquisitions", "Bente Jorgensen's Perfectly Paid Vacations Act", "Bente's Rarefied Seedling", "Bente's Travel Journal", "Berkeley Meat Sunderer", "Bertie Pinkie-Toff's Edict of Elitism", "Beryl's Barrel", "Beryl's Fosterling", "Beryl's Mink Coat", "Beryl's Nail Clippers", "Bespoke Tailor", "Bicycle Repair Kit", "Big Alcohol Crate", "Big Anarchist's Bomb Pack", "Big Bag of Money", "Big Box of Dynamite", "Big MagN8", "Bilberry", "Billiard Tables", "Biniam's Floodgate", "Bird-of-Paradise", "Birdhouse", "Biscuits", "Bison", "Black Bear", "Black Caiman", "Black Flag", "Black Flag", "Black Jaguar", "Black Powder Fireworks", "Black Rhino", "Black Swallower", "Blending Tank", "Blobfish", "Blotherhithe's Fusing Machine", "Blown-Apart 12-Pounder", "Blue Skies Delivery Service", "Blue Skies Janitor", "Blue Skies Maid", "Blueprint For An Infeasibly Large Ocean Liner", "Boilermaker", "Bomb", "Bombins", "Bone Adze", "Book-Binding Frame", "Boreal Caribou", "Bosun", "Botanist", "Bottlenose Dolphin", "Bovine Ivory Ring", "Bow Of The Lnu", "Bow Saw", "Boy's Choir Flyer", "Brass", "Brasserie Patron Mertens", "Bread", "Brewmaster Bill", "Brickmaker", "Bricks", "Brilliant Fireworks", "Broadcast Relay Station", "Bronze Helmet", "Bronze Performance Medal", "Bronze Sword", "Brother Hilarius, Purveyor of Monastic Mixtures", "Brown Bear", "Brownbeard's Cutlass", "Bruiser", "Bruno Ironbright, Engineering Giant", "Brón Dolmen", "Builder's Tools", "Bumm Brimmell, the Original Dandy", "Bureau of Worker-Owned Cooperatives", "Burner", "Burrstone", "Butcher", "Butterflies", "CTC Extinguisher", "CV and Cover Letter from 'Decorated General' Tutor", "CV and Cover Letter from 'Master Craftsman' Tutor", "CV and Cover Letter from 'Noble' Tutor", "Cabinet Maker", "Cabinet of Curiosities Pamphlet", "Cable-Backed Bow", "Cake Sale Flyer", "Calamari", "Calciner", "California Gull", "Calla Lily, Of The Desert Bloom", "Camellia Garden", "Camellia Garden", "Camellia Garden", "Camphor Wax", "Can Seamer", "Cane Saplings", "Canister Shot", "Canned Food", "Canner", "Cannoneer", "Canterbarry's Countryside Cruise Leaflet", "Caoutchouc Cable", "Caoutchouc Saplings", "Caoutchouc", "Capacious Waste Treatment Tank", "Caprice Ltd, The Monopoly On Local Construction", "Captain Felicity Phoenix, Back From the Grave.", "Captain Ibrahim", "Captain Kadijah", "Captain Moby, Old Dog of the Sea", "Captain Nineveh's Miraculous Lobster Trap", "Captain Richard Moses, Skipper of Schooners", "Captain Smith, Titan of the Sea.", "Captain of Marines", "Captains Of Industry, Vol IV", "Captains Of Industry, Vol XVI", "Captains Of Industry, Vol XXXI", "Caracal", "CarboN8r", "Careful Craftsman", "Caribou Meat", "Carpenter's Tools", "Carpenter", "Carpenter", "Carriage Wheel", "Cartographic Sketchbook", "Carved Stone Bear", "Cast Iron Stove", "Cast Mortar", "Cattle", "Cattle", "Cauldron of Fish Stew", "Cecilia the Coffeessieur", "Cellar Man", "Celluloid", "Cement", "Cemented Steel Shells", "Cementer", "Censored Books Act", "Ceramics", "Certificate of Industrial Standard", "Certificate of Inspection", "Chain Shots", "Champagne Taster", "Champagne", "Champak", "Chandler", "Chantelle the Charcutier", "Charcutier", "Charlotte the Chocoholic", "Chassis", "Cheetah", "Chef Michel", "Chemical Fertiliser", "Cherry Wood", "Chewing Gum", "Chewing Gum", "Chicken", "Chief George Doughty, Smouldering Hero", "Chief Superintendent Clifford", "Chimney Sweep Charter", "Chital", "Chocolate", "Chocolatier", "Chronicles of Enbesa", "Chronicles of Enbesa", "Chronicles of Enbesa", "Chronicles of Enbesa", "Chronometrist Chiara", "Church Tax Law", "Cigar Afficionado", "Cigars", "Cinnamon", "Circular Rip Saw", "Circular Saw", "Citrus", "City Map Act", "Clay Kudlik", "Clay Pipes", "Clay", "Cliffside Map", "Clockmaker", "Coal Burner Advertisement", "Coal Permit", "Coal", "Coastal Dredge", "Coastal Leopard", "Cobbler's Last", "Cocoa Beans", "Cocoa", "Coconut Oil", "Coconut Palm", "Coelacanth", "Coffee Beans", "Coffee Beans", "Coffee Sommelier", "Coffee", "Cognac", "Coins", "Coke", "Cola Mola Wola Machine", "Collection Of Lost Expedition Relics", "Collegiate Bill", "Colonel Barzagli", "Coloured Fireworks", "Colourful Textiles", "Combustion Chemist", "Comedian", "Commmodore Stephen Drake, The Besieger.", "Commodore's Medal", "Common Reed", "Communal Garden", "Concierge", "Condor", "Confectioner", "Confessor of the Burgeois", "Congrave Rocket", "Constable Chaos", "Contraception Regulation", "Controllable-Pitch Propeller", "Conveyor", "Copper Distiller", "Copper Ring", "Copper", "Copperplate Printer", "Corn Seeds", "Corn", "Cosmo Castelli, Agronomic Trailblazer", "Costume Designer", "Costumes", "Cotton Fabric", "Cotton Seeds", "Cotton", "Court of Laws", "Court-Sealed Map", "Craft Beer To Takeaway", "Crate of Convulsing Bait", "Crimbles Choral Society Poster", "Criminal Esoterica", "Cristobal Taffeta, The Trendsetter", "Critically-Acclaimed Tragedian", "Crocodile", "Cropper", "Curly-Hair Tarantula", "Cursed Mummy of Cabot", "Cyanide", "Damaged Statue", "Dandelion", "Daring Scuba Diver", "Dario the Mechanical Engineer", "Dean, the Dean of Deansville University", "Decaying Mummy", "DeciN8r", "Decreased Alcohol Tax Act", "Decreased Alcohol Tax Bylaws", "Dedicated Airship Hoist", "Deep Blue Ship Blueprints", "Deep-Sea Dragonfish", "Deke's Indispensible Dockyard Crate", "Delicatesseur", "Delores the Diplomat", "Demijohn of Milk", "Democratically Elected Minister", "Demonstration Ban", "Dennis Brammen, the Food Critic", "Depleted Treasure Chest", "Diamond", "Diary of Mekdem's Guard", "Dietary Advice", "Digger", "Dionysian Mosaic", "Dire Wolf", "Distiller", "Diver", "Djimon, the Melancholic Expatriate", "Dockyard Tool System", "Dockyard Toolbox", "Dockyard Toolchest", "Dog Fox", "Dog-Breeder", "Domestic Cat", "Domestic Dog", "Domestic Pig", "DomiN8r", "Donkey", "Double Canister Shot", "Double Iron Grillwork", "Double Redundancy", "Dr. Ali Al-Zahir, the Botanical Director", "Dr. Maurice Slim, Former Head of Promise Trust", "Dr. Mercier's Anarchy Poster", "Dr. Mercier's Feverishly Federating Flag of the Free", "Dr. Salvador", "Dr. Íñigo Flores", "Drae von Gast", "Drakkar Carving", "Draughtswoman", "Dredger", "Dressmaker", "Drew Durnil, the Speculative Historian", "Dried Meat", "Drift Net", "Driftwood Baleen Boat", "Driftwood Iggaak", "Drill", "Driller", "Drip Coffee Pot", "Dromedary", "Druid's Sickle", "Drum of Selamawi", "Drunken Dave's Distillery Dive Leaflet", "Dubious Treasure Map", "Dubious Treasure Map", "Dunnite Shells", "Dusky Dolphin", "Dynamic Loudspeaker", "Dynamite", "Dynamite", "Dynamite", "Dynamo", "Eagerly's Every-Fifth-Day-a-Half-Day Act", "Eagle of the Sceptre of Capon", "Eagle", "Early Closure Act", "Eastern Elephant", "Eastern Giant Snail", "Eastern Water Buffalo", "Echo Sounder", "Eduardo Bernal, the Father of Public Relations", "Eduardo the Expert Broker", "Edvard's Judicial Sentencing", "Elder Selassy'e", "Elderberry", "Electric Cable", "Electric Cables", "Elephant", "Elevator", "Elite Airship Gunner", "Elk", "Ella Golden, The Anarchist Luminary", "Elliot Blitz, the Security Engineer", "Elmer's Electric Oven", "Emperor Scorpion", "Enbesa's Bronze Age (Notes)", "Enbesa's Silver Age (Notes)", "Enbesan Animal Nutritionist", "Enbesan Bishop", "Enbesan Book-Binding Kit", "Enbesan Envoy", "Enbesan Fire Tamer", "Enbesan Wolf", "Encyclopaedia Universalis Vol. IV", "Endangered Butterflies", "Ensign", "Enthusiast Cook", "Entremetier", "Ermenegilda Di Mercante, Purveyor of Tall Ships.", "Ernest the Educator", "Eshe, the Eager Enbesan Entrepreneur", "Espeletia", "Essays on the Freedom to Love", "Ethanol", "Evangeliser's Tracts", "Evening Curfew", "Evil ExsanguiN8r", "Exchange Rate Increase", "Excommunication Act", "Exhaust Blower", "Expert Agricultural Textbook", "Expert Grazer", "Expert Hunter", "Expert Planter", "Exporter of Goods", "Extended Schooling Act", "ExtermiN8r", "Extra Ore Extractor", "Extra Virgin Oily Sucker", "Extractor of Ore-someness", "Extremely Loud Bell", "Eyeleting Machine", "Fabulous Feed Yard", "Factory Overcrowding Regulations", "Factory Regulations", "Fake Fur Frame Loom", "False Killer Whale", "Fans", "Fat Chunk Candle Maker", "Fedora", "Feed Yard", "Fellow", "Felt", "Fennec", "Feras Alsarami, the Persuader", "Ferhat the Experienced Builder", "Fernando de Faro, Coffee-Lover and All-Round Cad", "Fertiliser", "Field Marshall Matravers, Conductor of Armies.", "Filaments", "Film Reel", "Fine Cake Decorator", "Fine Couturier", "Fine Curing Chamber", "Fine Feed Yard", "Fine Fishing Net", "Fine Handheld Power Auger", "Fine Spyglass", "Fine Steel Hull", "Fine Wooden Hull", "Finery", "Fire Chief Mills", "Fire Extinguishers", "Fire Prevention Directive", "Fire Safety Poster", "Firefly Squid", "First-Rate Sapper", "Fish Oil", "Fish Stew", "Fish", "Fizgig's Fabulous Fireproofing Failsafe", "Fjordbuster Mk. VI Propulsion Array", "Flame Hose", "Flame Thrower", "Flamingo", "Flour", "Flower Girl", "Foam Extinguisher", "Fodder Supplements", "Foghorn", "Folk Tales", "Fool", "Footman", "Fore-and-Aft Rig Sails", "Forester Seed Collection (Limited Use)", "Forester Seed Collection (Persistent)", "Forester", "Forged Steel Shells", "ForkTuN8 Mk.1", "ForkTuN8 Mk.2", "ForkTuN8 Mk.3 - Very ForktuN8 Indeed", "Former Corsair", "Former Pirate", "Former Pyrphorian Whizz", "Foxglove", "Franck von Lewenstein, Warmest of Hosts", "François Strindberg, Crown Jeweller", "François Thorne, The Industrial Reinforcer", "Free Press Act", "Freedom For All Poster", "Freedom of Association Act", "Freedom of Faith Act", "Freight Elevator", "Fried Plantains", "Frozen Woolly Mammoth", "Fuel", "Fuel", "Fuel", "Fuel", "Fur Coats", "Furrow Plough", "Furs", "Gantry Crane", "Gantry Repair Crane", "Gasparov's Chest of Shame", "Geckos", "Gelada", "Gelatier", "General Ewing Thomson, Planter of Flags.", "General Foreman", "Generous Cheese Board", "Generous Innkeeper", "Gennaro Garibaldi, Chocolatier by Royal Appointment", "Gentian Garden", "Gentian Garden", "Gentian Garden", "George Smith's Statute of Universal Collaboration", "George's Ear Trumpet", "George's Police Badge", "George's Sewing Machine", "Georgina Duffy, The Call your Bluffy.", "Gerhard Fuchs, of the Patent Eyeglass.", "Germ Resistant Soap", "Get Rich Quick Vol I: Pestilential Profit", "Get Rich Quick Vol II: What Shape is a Banana Anyway?", "Get Rich Quick Vol III: Pretty Polyculture", "Get Rich Quick Vol IV: The Wasteland", "Get Rich Quick Vol IX: Paid To Perform", "Get Rich Quick Vol V: The Geppetto Gambit", "Get Rich Quick Vol VI: Night Moves", "Get Rich Quick Vol VII: Automation Nation", "Get Rich Quick Vol VIII: Backlogs On The Fire", "Ghebbi Diorama", "Giant Deer", "Giant Water Lily", "Gideon Small, Guru of Pirate Gunboats", "Giorgis, the Eminent Hymnodist", "Giraffe", "Glass Blower", "Glass Maker", "Glass", "Glassblower", "Glory of Kings", "Glossy Black Cockatoo", "Gløgg Splurger", "Goat Meat", "Goat Milk", "Goat", "Goblin Shark", "Goidelic Stone Circle", "Goidelic Stone Knot", "Gold Ore", "Gold Performance Medal", "Gold", "Golden Harvester From The Future", "Golden War Helm", "Goldsmith Gilbert", "Goods Duties Reduction Act", "Goose Feathers", "Gordon the Master Grocer", "Gorilla", "Goulash", "Governess", "Grain Seeds", "Grain", "Gramophones", "Gramophonist", "Grand Trading Charter", "Grandiloquent Copper Distiller", "Grape Vines", "Grapes", "Grapeshot Cast Mortar", "Grave Human Skull", "Graveyard Relocation Measures", "Great Auk", "Great Star Coral", "Great White Flag", "Great White Shark", "Greedy Mitts", "Greek Fire", "Green Turtle", "Green's Law to Postpone The Apocalypse", "Grey Crowned Crane", "Grey Wolf", "Grigor the Geologist", "Gritty Gas Extractor", "Grocer", "Grower", "Grubby Hands", "Guayacan de Manizales", "Guide", "Gungurru", "Gunner", "Gunsmith", "Gyroscopic Gold Panner", "Gyroscopic Torpedo", "HMS Hades Replica", "Haha, you're dead meat", "Half-Frozen Rats", "Hall of Collective Property", "Hall of Mutualist Ideologies", "Hand-Blown Grenade", "Hand-Pollination Method", "Handicrafter", "Handler", "Hans Eichendorf, A Wholesale Success", "Hans Gun", "Hans Klein, Old Grandfather Time", "Happy Homesteader", "Harker's Electric Treadle Lathe", "Harlow Family Heirloom", "Harlow Family Ring", "Harvest Fayre Flyer", "Harvester", "Hat Maker", "Hatter", "Headmistress", "Headteacher's Charter", "Heat Activated CTC Grenade", "Heat Exchanger", "Heated Jacket Of Pouchiness", "Heavy Coal Permit", "Heavy Drill", "Hedge Maze", "Hedge Maze", "Hedge Maze", "Heimskringla", "Helical Planer", "Helium Expert", "Helium", "Helmet Jellyfish", "Hemp", "Henri Zanchi, Man of Steel", "Herbal Hygienist", "Herbarium", "Herbs", "Herdsman", "Hervé Savonne, Suppressor of Grime", "Hevea Tree", "Hibiscus Garden", "Hibiscus Garden", "Hibiscus Garden", "Hibiscus Petals", "Hibiscus Tea", "Hide Scraper", "Hieromonk Bede's Cheeky Little Tithe Law", "High Fees Law", "High-Powered Scope", "Hilarious Helium Party Balloon Pump", "Hippopotamus", "Historical Preservation Act", "Hive Smoker", "Hobby Diver", "Hogarth the Harbourmaster, Veteran Of Trelawney", "Hollow-Tree Drum", "Holy Moly", "Hop Seeds", "Hops", "Hor Statue", "Horse", "Horticulturalist Hermann", "Hot Sauce", "House Doctor", "Housemaid", "Huge Alcohol Crate", "Huge Bag of Money", "Human IncarN8 Nr. 9", "Huskies", "Husky Sleds", "Hybrid Cane Saplings", "Hybrid Caoutchouc Saplings", "Hybrid Cocoa Beans", "Hybrid Coffee Beans", "Hybrid Corn Seeds", "Hybrid Cotton Seeds", "Hybrid Grain Seeds", "Hybrid Grape Vines", "Hybrid Hop Seeds", "Hybrid Oyster Spawn", "Hybrid Plantain Saplings", "Hybrid Potato Sprouts", "Hybrid Red Pepper Seeds", "Hybrid Tobacco Seeds", "Hydraulic Crane", "Hydraulic Repair Crane", "Hydroelectric Wheel", "Hydrostatic Torpedo", "Ice Cream", "Ice Maker", "Iconography of the Human Mind", "Idol of Hueyaca", "Igluit", "Iguaca", "Illuminated Manuscript", "Illuminated Script", "Illustrious Gemologist", "Impala", "Imperial Eagle Mosaic", "Imperial Figurehead", "Imperial Zebra", "ImpersoN8r Nr. 531", "Import Tax Reduction Act", "Incendiary Grenades", "Incendiary Grenades", "Incendiary", "Indigo Dye", "IndiscrimiN8r", "Industrial Binding Machine", "Industrial Chemist", "Industrial Lubricant", "Industrial Thresher", "Industrial Toxin Filter", "Industrious Embroidress", "Inflammable Concoction", "Influence", "Inma's International Trade Fair Billboard", "Inspector", "Interspecies Grafting", "Intoxicator", "Intricate Baked Mud Tile", "Inuinnaq Case", "Inuit Hunter", "Inuit Seamster", "Inunnguaq Inuksuk", "Iris Garden", "Iris Garden", "Iris Garden", "Iron Factory Bell", "Iron Founder", "Iron Grillwork", "Iron Helm", "Iron Oven", "Iron Shield", "Iron Sword", "Iron and Glass Grillwork", "Iron", "Island Charter", "Itsuko the International Exporter", "Ivory Gull", "Ivory Iggaak", "Ivory Kudlik", "Jaafan the Cruise Impresario", "Jade Pearl", "Jaguar", "Jakob Sokow, The Charitable Banker", "Jalea", "Jam", "Jar of Worms", "Jean's Little Helper", "Jellyfish", "Jerusalem Sage", "Jet Propeller — Cyclone", "Jet Propeller — Hurricane", "Jet Propeller — Squall", "Jeweller", "Jewellery", "Jittery Pyromaniac", "Johan the Inventor", "Joiner", "Jorgensen Trust Manifesto", "Joseph Beaumont, Historic Society Founder", "Journal of Common Thought", "Journal of Popular Thought", "Juanita the Mummy", "Jukebox", "Jörg von Malching, Augur of the Auric", "Jörmungandr's Lair", "Kaldi, Infuser Of Teas", "Kantyi of the Quinoa", "Kaputaq Awl", "Ketema's Cousin", "Ketema's Industrial Kiln", "Khadija Tsegaye, Angereb's Market Master", "Kidusi Anitoni Port Registry", "Kidusi Church Diorama", "Kiln Keeper", "King Wag's Last Testament", "Kingfisher", "Kiosk of Flighty Fortunes", "Kitchen Help", "Knitter", "Kolponomos", "Krapotkin, Evolutionist of Kindness", "Kyria's Old Diary", "La Condesa Del Amor's Majestic Dance Billboard", "La Tribune Libre", "Lacquer", "Lader", "Lady Banes, Proponent of Protectionism.", "Lady Cerebellum's Ickle Bickle Classrooms Charter", "Lady Dumbedown's Colossal Class Size Charter", "Lady Hunt's Advice on Being A Patient Spouse", "Lady Hunt's Order of Silence", "Lady Jane Smythe, Queen of Haute Couture", "Lady Lu, the Redeemed Pirate", "Lake Orangutan", "Lalla Mazigh, Merchant of a Free Market", "Lamie The Wistful Pirate", "Landscape Planner", "Lanterns", "Lapidary", "Large Communal Garden", "Large Looting Net", "Lateen Style Sails", "Lax Water Treatment Law", "Le Tocsin", "Leading Electrical Engineer", "Leather Boots", "Lector", "Lecturer", "Leghold Trap", "Lemonade", "Lender", "Length of Rope", "Lens Designer", "Letter of Immunity", "Letter of Marque", "Light Bulbs", "Lily Garden", "Lily Garden", "Lily Garden", "Lily the Fashion Designer", "Line Fisherman", "Lineman", "Lineman", "Linen", "Linseed", "Lion of Selamawi", "Lion's Mane Jellyfish", "Lion", "Lionfish", "Little MagN8", "Livestock Farmer", "Lobster", "Lobsterman", "Local Mail", "Local Stamp", "Long Range Bottom Trawl", "Lord Farquahar's Alcohol Tax Amendment", "Lord Footprint's Gargantuan Smokestack Act", "Lord High Admiral Nadasky, The Astute.", "Lord Stringer's Stringent Usury Laws", "Lord Wainthrop's Booze Taxationations Most Severe!", "Lost Expedition Scrap", "Louis P. Hecate, Arm-Puncturing Pioneer", "Lovelyman's Grinding Machine", "Loving Cattleherd", "Lucius' Long-Lost Doctrine of the Faith", "Luggage Size Regulations", "Lumberjack", "Lungnibbel Ring", "Machinist", "Mad Mary, Dynamite Enthusiast", "Mad Monkey's Incisor", "Madame Blanc, Maître d'hôtel", "Madame Kahina's Executive Tourism Drive", "Magan, the Master Craftsman", "Magnetist", "Magnifying Glass", "Magnifying Glasses", "Mai Kalan, Descryer Of Brilliant Jewels", "Maimer of Mustelids", "Major Cyclideon Exhibition: Literature", "Major Cyclideon Exhibition: Science", "Major Income Tax Amendment", "Major Tourism Campaign", "Malching's Back-to-Back Itinerary for Serfs", "Malt", "Mandatory Mass Law", "Manta Ray", "Map Provided By Imperial Sailors", "Map Taken from an Insurance Report", "Map from a Recent Shipwreck", "Map of Ancient Ruins", "Map of Isabel Sarmento's Emergency Vault", "Map of Old Nate's Prototype Airship Crash Site", "Map of Sile'Amidos", "Map of Vicente Silva's Sunken Ship-of-the-Line", "Map of Willie Wibblesock's Wondrous Stash", "Map of a Barrier Reef", "Map of a Coral Garden", "Map of a Domestic Skirmish", "Map of a Forgotten Wreck", "Map of a Ghost Ship Sighting", "Map of a Huddlesome Bay Trading Post", "Map of a Kelp Forest", "Map of a Lost Shepherds' Camp", "Map of a Lost Temple", "Map of a Rebel Incursion", "Map of a Remote Trading Post", "Map of a Seagrass Meadow", "Map of a Seal Feeding Ground", "Map of a Sunken City", "Map of a Sunken Estate", "Map of a Sunken La Corona Clipper", "Map of a Sunken Rebel Supply Ship", "Map of a Sunken Royal Clipper", "Map of a Trelawney Skirmish", "Map of an Abandoned Inuit Village", "Map of an Abyssal Zone", "Map of an Ancient Naval Battlefield", "Map of an Ancient Outpost", "Map of an Arctic Basin", "Map of an Inuit Dispute", "Map of an Oceanic Trench", "Map of an Underwater Pyramid", "Marcel Forcas, Celebrity Chef", "Marco de la Mocha, Crusher of Beans", "Maria Maravilla", "Maria Pizarro, Maker of Pirate Monitors", "Mariana the Master Stylist", "Marie-Antoine, Pâtissier Royale", "Marie-Louise Carême, Chef de Cuisine", "Marigold Garden", "Marigold Garden", "Marigold Garden", "Marine Biologist", "Marine Biologist", "Mark van der Mark, Breeder of Shepherd Dogs", "Marsh-Mallow", "Mason", "Massive MagN8", "Master Confectioner", "Master Craftsman Franke", "Master Craftsman Morris", "Master of the Forges", "Masu Masu's Trinket", "Masu's Mapamundi", "Mate", "Mathematician", "Mathilde the Parrot", "Maxime Graves, Delicatesseur Extraordinaire", "Maxime Renard the One-Man Show", "Meadow Sage", "Meaningless Bling", "Meat Guillotine", "Mechanic", "Mechanical Drill", "Mechanical Loom", "Mechanical Panner", "Mechanised Glass Blower", "Medal-Winning Producer", "Medicine", "Medium Looting Net", "Megalodon's Abyss", "Megaphone", "Melancholic Artisan", "Melinite Shells", "Melting Point Apparatus", "Menhirs", "Merchant", "Mercier's Cookbook", "Mercier's Manifesto of the Free", "Mercier's Megaphone", "Mercier's Separate Church and State Clause", "Mercury Thermometer", "Mesoan Calendar", "Mesoan Glyphs", "Mesoan Harbour", "Mesoan House", "Mesoan Temple", "Metaxya Fern", "Mezcal", "Mezoan Horn Cone", "Micaela the Mining Engineer", "Mickleton's Hydraulic Compacting Press", "Microbiologist", "Midnight Curfew", "Midshipman", "Miles the Master Butler", "Military Logistician", "Milk", "Millicent's Manifesto of the Suffragette", "Milliner", "Millstone", "Mined Quartz", "Miner", "Mineralogist", "Minerals", "Minimum Schooling Act", "Minimum Working Age 6 Act", "Minimum Working Age 8 Act", "Minor Cyclideon Exhibition: Agriculture", "Minor Cyclideon Exhibition: Industry", "Minor Income Tax Amendment", "Minor Tourism Campaign", "Miraculous Steel Plough", "Miserable Max's Brutal Tax Legislation", "Miss Jorgensen's Exemplary School-to-16 Proposal", "Miss Rodriguez, the Conservationist", "Mixer", "Mme. Elise The Nose Bouquet", "Moderate Alcohol Tax", "Moderation Act", "Modern Ship Tools", "Modiste", "Mole Master", "Monkey-Tooth Amulet", "Moon Orchid", "Moon Pool Diving Bell", "Moonshiner", "Mordant Conservative Playwright", "Morning Glory", "Mortar", "Morticromicron", "Mosasaurus Fossil", "Mosquito Net", "Mother and Child Figure", "Mother of Pearls", "Mother-In-Law's Tongue", "Motor", "Moulder", "Mountain Nyala", "Mr. Bertram, Hotel Manager", "Mr. Garrick, Founder of the Entrepreneur Gentlemen's Club", "Mr. Slippery's Splendiferous Sewage Proposal", "Mr. Swell, the Hydrodynamist", "Mrs. Beetle's Repulsive Law on Pestilence", "Mrs. Brown the Cultivator", "Mrs. Mayson, The Very Good Housekeeper", "Mrs. O'Mara's Marvellous Tax Relief Regulations", "Ms. Amelia's Essential Prohibition Measures", "Mssr. Fontaine's Street Signage Solution", "Mud Bricks", "Mulatu, the Musical Maestro", "Mulcher M1897", "Mullivann's Multi-Shot Mortar", "Multiple Expansion Steam Engine", "Mummy Bandages", "Music Box", "Musk Ox", "Mystery Vending Box", "N.8.D.A.R", "Naaqtuuq the Sled-Builder", "Nadasky's Journal (Incomplete)", "Nadasky's Journal (Near Complete)", "Nadasky's Journal", "Nadasky's Medal", "Nadia the Spotter", "Nandu Feathers", "Nandu Leather", "Narwhal", "Nate's Darling Kettle", "Nate's Experimental Illuminations", "Nate's Magically Morphomatic Rockets", "Naval Architect", "Nazcan Tomb", "Neanderthal Skeleton", "Net Trap", "New World Science Reporter", "Nice Scrap", "Night Pollution By-Law", "Night and Day Pollution By-Law", "Non-Functional Sea Mine", "Noon Closure Act", "Norfolk Kaka", "Nurse", "Nuyakpak", "Ocean Plotting Sheet of the Southern Hemisphere", "Oceanic Dredge", "Oil Lamps", "Oil", "Oily Sucker", "Oily Super-Sucker", "Old Factory Bell", "Olympian Temple Remains", "On Civil Disobedience", "On the Inferior Nature of Women", "One-Man-Band Billboard", "One-Turn Screw Propeller", "Optician", "Optimised Automatic Loom", "Optimized Pruning", "Optometrist Otto", "Orangutan", "Orca", "Orchid", "Ore Extractor", "Orichalcum Statue", "Oriel's Automatic Blowing Machine", "Oriental Cherry", "Oriental Fireworks", "Oriental Tassel Fern", "Original Vanity Screen Advertisement", "Ornate Candles", "Orvette Charles, of The Royal Society of Oceanographers.", "Oryxe", "Ostrich", "Outdoor Cook", "Outdoorsy Grill", "Ox", "Oxford Electrical Bell", "Oyster Spawn", "Painted Mask", "Painted Stones", "Palm Tree", "Pantry Chef", "Pantry Maid", "Papal Paper of Prenatal Preservation", "Paper", "Papyrus", "Parish Tax Law", "Park Ranger", "Parkas", "Parrot", "Particularly Pull-y Pulley", "Pastry Chef", "Pastry Cook", "Patent Steel Seed Drill", "Pater Dekama", "Patriarch Grigor's Calendar of Holy Days", "Patriarch Matteos X", "Patriarchy, the Root of Class Struggle", "Paul Poiret, Shocking Fashion Designer", "Peacock", "Pearl Diver", "Pearls", "Peddler", "Pedologist", "Pedro, Captain of the Panama", "Pelican", "Pemmican", "Penicillin", "Penny Farthings", "People's Jury Hall", "Peppermint", "Perfumes", "Perfumier Prunella", "Perry's Giant Sunburst", "Pertwee's Turbo Generator", "Peséos", "Peter Flinders, Conservator of Artefacts", "Petra Piper, Fashioner of Pirate Frigates", "Phoenix Egg", "Phosphorus Dissipation Recipe", "Physicist", "Piano Concerto Billboard", "Picker", "Pietro Jonah Proud, The Philosopher of the Public Good", "Pigeon", "Pigments", "Pigs", "Pillar Coral", "Pilot", "Pink River Dolphin", "Pink Sugarbush", "Pirate Captain's Black Flag", "Pirujaqarvik Inuksuk", "Pititsi", "Planer", "Plantain Saplings", "Plantains", "Plate Heat Exchanger", "Ploughman", "Poacher", "Pocket Watches", "Poetry Subscription", "Polar Bear", "Police Equipment", "Political Club Dissolution Order", "Polyglot Scholar Rahim", "Pommel of the Sceptre of Capon", "Ponchos", "Pope Lucius I, The Rejuvenator", "Poppinjay's Pulleyed Patcher-Upper", "Poppy", "Popular People's Court", "Port Dealer", "Porthampton Mass Conveyor", "Postal Banker", "Poster of the Leader", "Potager", "Potato Sprouts", "Potatoes", "Potholes Decree", "Potter's Wheel", "Power Loom", "Practised Inuit Seamster", "Precious Royal Jewellery", "Precision Horologist", "Prickly Sea-Devil", "Pride of Madeira", "Priest", "Primitive Fabrics", "Printing Press", "Proclamation of Knighthood", "Prof. Iwa Ebashi, Pioneer of the Radioactive", "Prof. Ram Devi, The Bundle of Energy", "Professor Razzaq", "Prospector Of Auspiciousness", "Prospector", "Prosperity Contract", "Protoceratops Eggs", "Prototype Torpedo", "Public Campaign: Eat those Proteins!", "Publican", "Puma", "Punctilious Physician", "Purple Pitcher Plant", "Purple Saxifrage", "Pygmy Hedgehog", "Pygmy Killer Whale", "Pyrite Crystals", "Pyrphorian Calciner", "Pyrphorian Flame Thrower", "Pyrphorian Machine Instructions", "Pâtissier Patrice", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "QI_Treasure_Dummy", "Qajak", "Qing's Eyeglasses", "Qing's Volume of Poetry", "Quality Cane Saplings", "Quality Caoutchouc Saplings", "Quality Chocolatier", "Quality Cocoa Beans", "Quality Coffee Beans", "Quality Corn Seeds", "Quality Cotton Seeds", "Quality Grain Seeds", "Quality Grape Vines", "Quality Hop Seeds", "Quality Oyster Spawn", "Quality Plantain Saplings", "Quality Potato Sprouts", "Quality Red Pepper Seeds", "Quality Tobacco Seeds", "Quarry Foreman", "Quarryman", "Quartz Crystals", "Quartz Sand", "Queen of the Andes", "Queen's Birthday Decree", "Queenby Apiary Suit", "Quipu", "Rabies Vaccine", "RacioN8d Super Spinner", "Radiator", "Railroad Crane", "Railway Repair Crane", "Rare Artifacts Pamphlet", "Rats", "Re-Rendering", "Rear Admiral Richard B. Sharpe, The Eagle-Eyed", "Recipe Archivist", "Recycling Expert", "Recycling Specialist", "Red Deer", "Red Fox", "Red Pepper Seeds", "Red Peppers", "Red Tiger Lotus", "Red White Petunia Garden", "Red White Petunia Garden", "Red White Petunia Garden", "Refined Fertiliser", "Refined Fodder Supplements", "Refined Industrial Toxin Filter", "Refined Water Distiller", "Refiner", "RefridgerN8r", "Regal Gold Ring", "Regional Mail", "Register of the IXth Regiment", "Registration Confirmation: Stage One", "Registration Confirmation: Stage Three", "Registration Confirmation: Stage Two", "Reinforced Bulkhead", "Reinforced Concrete", "Religious Schooling Charter", "Renowned Excavator", "Repairman", "Researcher's Old Map", "Resin", "Respected Oenologist", "Reverberatory Furnace", "Rhododendron", "Richardson's Log", "Right of Protest Act", "Ringed Seal", "Rioter Amesty Law", "Rioter Immunity Act", "Roaster", "Rocket Barrage", "Rocket Salvo", "Rod of the Sceptre of Capon", "Rodrigo the Ranchero", "Rohit Bhargava, the Naval Architect", "Royal Airship Stamp Mimic", "Royal Crown", "Royal Letter of Immunity", "Royal Letter of Marque", "Royal Letter", "Royal Palm", "Royal Trading Charter", "Rudimentary Necklace", "Rugged Outdoor Cook", "Rum Blender", "Rum Runner", "Rum", "Rusty Sestertius", "Réis", "Sabbath Repeal Law", "Sacred Blue Lily", "Safer Roofs Fund", "Safety Elevator", "Sailor", "Sails", "Saint D'Artois, Vision of the Valley", "Sakku", "Sales Clerk", "Salima the Shipping Operator", "Salt", "Saltpetre", "Salvador, the Stalwart", "Sanctuary of Hellebore", "Sandro the Nature Chef", "Sanga Cow", "Santillana Cave Painting", "Saponification Tax", "Sarah Bartok, The Golden Voice", "Sausages", "Savvy Customs Officer", "Scammer", "Scara the Marine Biologist", "Schnapps", "School Discipline Charter", "Schoolteacher", "Science Whizz", "Scooter", "Scrap", "Scrap", "Sea Heart Liana", "Sea Mine", "Sea Thrift", "Sea-lavender", "Seafood Stew", "Seagull", "Seahorses", "Seal Skin", "Seamstress", "Seasoned Salter", "Secularised Bylaws", "Seraphim Papadikas, The Window Dresser", "Serendipitous Tombola of Inexplicable Curiosity", "Sergeant", "Sergeant", "Sewing Machines", "Sewing Threads", "Sgt. Nicolas Gabriel, the Over-Achiever", "Shampoo", "Sharp Sled Builder", "Shayi Shaah, Tea House Empress", "Sheep", "Sheet Music Anno 1404", "Sheet Music Anno 1701", "Sheet Music Anno 1800", "Sheet Music Anno 2070", "Sheet Music Anno 2205", "Shepherd", "Shih Yang, Sirer of Pirate Ships-of-the-Line", "Shiny Pearls", "Ship Builder", "Ship Piston Engine", "Ship Tool Chest", "Ship Tool Trolley", "Ship Toolbox", "Ship Tools", "Ship Turbine", "Ship's Chaplain", "Shipper", "Shipwrecked Captain's Map", "Shroud of the Great King", "Silken Fabrics", "Silva's Tortilla", "Silver Amalgam", "Silver Bars", "Silver Coins", "Silver Helmet", "Silver Necklace", "Silver Ore", "Silver Performance Medal", "Silver Ring", "Silver Shield", "Silver Sword", "Silverback Gorilla", "Simmons Electrifying Elevator", "Simon Sarmento's Sword", "Singh's Steel Double-Hull", "Sir Archie's Royal Minty Printy Thingy", "Sir Charles Rafferty, Metropolitan Commissioner", "Sir Lewis Brindley, the Chemist", "Sir Milfoy Credenza-Belfry-Guscott, HM Ship Architect", "Slave Owner's Ledger", "Sled Builder", "Sleds", "Sleeping Bags", "Slinky Sinistra, Warlock of Pyrphorian Warships", "Sluggish Surge-Wheel", "Slurry Pit", "Slurry Tank", "Small Alcohol Tax", "Small Looting Net", "Smoke Detector", "Snow Gum", "Soap Maker", "Soap", "Soapstone Kudlik", "Soccer Balls", "Soda-Acid Extinguisher", "Soft Drink Seller", "Soil Sampler", "Sole-Searcher's Stitcher", "Sommelier Raymond", "Song of Ilion", "Song of Odysseus", "Sophisticated Slurry Lagoon", "Sous-Chef", "Southern Gull", "Souvenir Seller", "Souvenirs", "Spare Wheel", "Sparky", "Spartan Schooling Charter", "Spear of Selamawi", "Spear of Selamawi", "Special Scrap", "Spectacle-Maker", "Spectacled Bear", "Spectacles", "Sperm Whale", "Spice Master", "Spiced Flour", "Spices", "Spitting Cobra", "Spoondrift — Nathaniel's Pressurised Diving Bell", "Sports Gondola", "Spotted Dolphin", "Spotted Hyena", "Square Rig", "Staghorn Coral", "Stainless Hydroelectric Wheel", "Standard Pirate Flag", "Star Jasmine", "Statue of Anubis", "Statue of Enesidaone", "Statue of a Jade Dragon", "Statutory Night Work Act", "Statutory Twelve Hour Day Act", "Steam Carriages", "Steam Engineer", "Steam Motors", "Steam Powered Brewing Machine", "Steam Turbine", "Steel Beams", "Steel Moulds", "Steel", "Steely-Eyed Steelsmith", "Stegosaurus", "Steven MacLeod, Geological Surveyor", "Steward", "Sticky Shiny Claws", "Stilton Snood, Purveyor of Pyrphorian Monitors", "Stone Statue", "Stop Gate", "Store Greeter", "Stork", "Strange Serum", "Strange Talisman", "Striking Terracotta Figure", "Strongarm's Disappearing Gun", "Subalpine Fir", "Submarine Oscillator", "SubordiN8 Nr. 360", "Sugar Cane", "Sugar", "Summit of Atlantis", "Sunday Best Law", "Sunday Roast", "Superintendent", "Superphosphate Fertiliser", "Susannah Brightwoman, A Glimmer In The Darkness", "Susannah the Steam Engineer", "Suspended Frosted Glass Grenade", "Suspicia Slyhood, Begetter of Pyrphorian Battle Cruisers", "Swan", "Swift", "Swordfish", "Synthetic Chemist", "Sæmundar Edda", "T-Rex", "TNT Bomb", "Taboo Books Act", "Tailor", "Tailored Suits", "Takal's Necklace", "Taliriktug The Husky-Voiced", "Tallow", "Tambacto Diorama", "Tanner", "Tapestries", "Tara Brooch", "Tarnished Silver Coins", "Tatiana the Taste-Tester", "Tattered White Flag", "Tax Rebates", "Tax Reduction Legislation", "Taxman Tim", "Tea Selector", "Technician", "Teetotalers Manifesto", "Teff", "Telemobiloscope", "Telephone Repeater", "Telephones", "Temperate Pedologist", "Temple of Enesidaone", "Temple of Nafir Diorama", "Temple of Poseidon", "Ten-String Krar", "Teutonic Technical Secateurs", "The Ark of the Covenant", "The Art of Medicine", "The Art of Rule", "The Banyan Tree", "The Bish Bang Bibbly Experimental Display", "The Bizarro-Balloono- Magn8", "The Bizarro-Balloono-Magn8", "The Brazil Nut Tree", "The Capitalist", "The Country Bumpkin Cigar Roller", "The Cutting Edge", "The Don't Leave It 'Til L8r! EspressoN8r", "The Elusive Fern Flower", "The Epic of the Land of Lions", "The Epic of the Land of Lions", "The Epic of the Land of Lions", "The Epic of the Land of Lions", "The Exquisite", "The Firebrand and the Free", "The Gates to Paititi", "The Hades Asphodel", "The Herdsman's Journal", "The Historical Sophisms of the Doctrinaire School of Communism", "The Holy Book", "The Hopedale Loom", "The International Liberal Forum on Art and Craft", "The Jujube Lotus", "The Kaleidoscopeum Quarterly Review", "The Kraken's Pit", "The Kraken", "The Lord's Blessing", "The Magnificone Ice Cream Maker", "The Mine-o-taur", "The Nest of Gukulmatz", "The Old Pirate Hat", "The Palaces of Baiea", "The Portal to Hyperborea", "The Pulltoddle Freedom of Assembly Act", "The Rings Of The Kings", "The Sceptre of Capon", "The Subjection of Women", "The Treasure of Ben Sahid", "The TurbiN8r", "The Uncanny Pocket Cannery", "The Valley of Death: A Treatise on the Upper Kingdoms.", "The Von Malching Family Pennant", "The Von Malching Family Seal", "The Watchmaker's Loupe", "The Withdrawal Amendment", "Thielemann's Tube", "Thomasina Langton, Promoter Extraordinaire!", "Thundering Loudspeaker Network", "Tiger-Spotted Stanhopea", "Tiger", "Timber", "Time Bomb", "Timothy's Blurry Law of Utter Depravity", "Tinned Fish", "Titan Arum", "Tlayolotl Savor, King of the Corn", "Tobacco Blender", "Tobacco Seeds", "Tobacco", "Toklo the Trapper", "Toll and Loan Rates Increase", "Tools", "Topiary Avenue", "Topiary Avenue", "Topiary Avenue", "Topiary Garden", "Topiary Garden", "Topiary Garden", "Torcedor Lucia", "Torpedo", "Tortillas", "Tortillera", "Tower Of Jewels", "Toy Horse", "Toy Qamutiik", "Toys", "Trading Charter", "Transcript of Oracle's Etchings", "Trap", "Trapper", "Trash Filter", "Travel Agent", "Treasure Seeker", "Treasures of Sile'Amidos", "Treatise on Freedom of Speech", "Tree Surgeon", "Trees", "TrepanN8r", "Tropical Pedologist", "Trunnion Mortar", "Tub of Tasty Tubeworms", "Tugboatman", "Tulsi", "Tuned-In Radio Operator", "Twenty-Thousand Days Around The Centre Of The Earth", "Two-Horned Chameleon", "Two-Man Saw", "Two-Turn Screw Propeller", "Typewriter", "UltiN8 Power Matrix", "Ulu", "Umiak", "Unaaq", "Unflattering Portrait of Artur", "Unknown Luminescent Fish", "Ursula Green, Guardian of the Forests", "Uummannaq Mummy", "Valentina Álvarez", "Valkyrie's Helmet", "Valojoulx Cave Painting", "Vandalised Vanity Screen Advertisement", "Vaquita", "Vasco's Honourable Agreement", "Vase of Sile'Amidos", "Vegetable Grower", "Velocipedalist", "Ventilator", "Very Senior Lecturer", "Veteran Firefighter", "Veteran Whaler", "Veterinary Antibiotics", "Vice-Admiral Adams", "Vice-Admiral Romanov", "Vice-Almirante Gonçalves", "Vice-Almirante Paulista", "Vicente's Informant", "Victor Perfecto, Cigar Daddy", "Vincent Drag Saw", "Vintner", "Violins", "Viridescent Billiard Table", "Vitamin Supplements", "Volunteer Fireman", "Von Malching's Labour Law Proposal", "Von Malching's Minimum Working Age 4 Proposal", "Waha Desher's Workers", "Wahenoor, the Paper Mill Pro", "Walrus", "Walter Waterlogged's Wine Wanderings Leaflet", "Wanza Timber", "Waste Treatment Act", "Waste Treatment Tank", "Waste Water", "Watchmaker", "Water Cooler", "Water Distiller", "Water Hyacinth", "Water Lily", "Water Purifier Filters (Limited Use)", "Water Purifier Filters (Persistent)", "Water-Carrier", "Water", "Weapons", "Weaver", "Welder", "Whale Oil", "Wheelwright", "White Flag", "White Peacock", "White Tiger", "White Tooth, Sword of Rhydderch", "Wibble Soc. Sorcery Association Membership and Tarot Set", "Wild Abyssal Dredge", "Wild Anarchist Sacchetti", "Wild Anarchist Vacco", "Wild Dog", "Wild Frontiersman Steen", "Wildebeest", "Willa Cochran, Ace Pilot", "Willie's Autobiography", "Willie's Bar of Gold", "Willie's Completely Ordinary Wooden Box", "Willie's Sausage Ring", "Wilma the Wild, Bearer of Barracudas.", "Window-Dresser", "Windows", "Wine Grower", "Wise Enbesan Homesteader", "Wolf Pup Mummy", "Wood Ash", "Wood Veneers", "Wood", "Wooden Adze", "Wooden Crane", "Wooden Moulds", "Wooden Repair Crane", "Wooden Ring", "Wooden Shield", "Wool", "Work Clothes", "Work Clothes", "Working Hours Adjustments", "Writing Machine", "Wrought Ironsmith", "Yebeba's Sturdy Greenhouse", "Yellow Daisy Garden", "Yellow Daisy Garden", "Yellow Daisy Garden", "Yellow Lotus", "Yvonne the Yeowoman", "Zappy Battery #2205", "Zinc", "Â§--24890--Â§" };
static const char* guids[] = { "191853", "134715", "134705", "192213", "192240", "133321", "104896", "104933", "104934", "104935", "191761", "192207", "133322", "192362", "192336", "191821", "192483", "125122", "190711", "120016", "191867", "190728", "125066", "191610", "112825", "24151", "24107", "24023", "24163", "125052", "24014", "24044", "112556", "1010223", "125132", "111175", "190687", "2833", "4133", "4132", "4277", "2232", "4117", "124374", "191507", "191106", "193192", "191922", "193187", "24065", "113907", "192198", "192196", "192199", "120036", "191113", "190516", "112374", "838", "121004", "129022", "4134", "5399", "190483", "191788", "111039", "111033", "192312", "110965", "190482", "130777", "111108", "190237", "190868", "190615", "24061", "124869", "1964", "192465", "191568", "192236", "192237", "113361", "110994", "110986", "110995", "111176", "190478", "191907", "116097", "190504", "24655", "24372", "118137", "24068", "190690", "190717", "191719", "115469", "1772", "112706", "4272", "191086", "192440", "191470", "124875", "192028", "116133", "111127", "191709", "116149", "191752", "191433", "192215", "192216", "116159", "112387", "110950", "190888", "125142", "116143", "112824", "112478", "1010257", "1010258", "25131", "113354", "190511", "190536", "131223", "116183", "191889", "191381", "191925", "191108", "7225", "190628", "190860", "192223", "112554", "112548", "190464", "191800", "191576", "192228", "125121", "192006", "190476", "190669", "24114", "24100", "191085", "836", "191367", "112695", "101897", "124060", "190716", "190832", "1010193", "1010214", "114370", "111179", "136885", "191802", "192190", "192193", "125050", "191725", "192206", "192358", "192204", "192205", "125137", "192033", "191923", "110966", "191926", "192016", "112562", "114540", "135232", "125053", "191093", "125058", "135229", "191077", "191072", "191091", "192173", "193903", "190004", "191969", "124164", "112812", "8155", "112813", "125067", "191868", "137797", "137799", "137798", "192214", "190699", "191999", "120037", "116101", "125044", "191075", "191447", "24077", "190449", "124867", "116080", "190850", "191706", "1010204", "190673", "1010213", "191382", "191461", "1010205", "191971", "125072", "190523", "137288", "190526", "191383", "191052", "190551", "190664", "191422", "190515", "191980", "125139", "110943", "190740", "190844", "191384", "190454", "191915", "119217", "119216", "119215", "191429", "191703", "116079", "191705", "5380", "190830", "191071", "125116", "114857", "114869", "114870", "134616", "8154", "190931", "191964", "1010217", "191394", "191434", "132847", "112555", "190943", "1010255", "24018", "191755", "191451", "190766", "191450", "190622", "125057", "191452", "191453", "191600", "111026", "111027", "111028", "124157", "112576", "125126", "112694", "191979", "190694", "191425", "192211", "129663", "115471", "190892", "192165", "1010193", "191058", "192031", "191370", "190645", "135150", "1010231", "191958", "191462", "191771", "118724", "191676", "191675", "192005", "190647", "120016", "114388", "191315", "191401", "191403", "191333", "1010211", "124160", "191406", "190863", "135087", "135186", "24824", "191059", "190658", "191009", "191751", "191073", "1010258", "191331", "130937", "130938", "130939", "130945", "191323", "191742", "190641", "1010259", "133093", "190909", "190908", "133097", "191691", "116081", "114414", "1010201", "129238", "191321", "191754", "191684", "1010226", "112567", "191110", "125068", "190940", "1010254", "133095", "111121", "113469", "120031", "190946", "191369", "120032", "135234", "1010017", "24821", "116185", "114637", "191723", "191435", "191970", "190528", "958", "190727", "191611", "113535", "111112", "24054", "136699", "191069", "191330", "111162", "191852", "8417", "191816", "191831", "190887", "190894", "190553", "1010230", "110937", "190949", "120034", "190752", "191348", "5389", "1010240", "190937", "1010253", "112484", "129236", "116182", "112560", "191707", "104639", "191347", "190731", "191506", "190745", "190461", "192355", "24852", "190525", "114539", "190620", "191424", "125143", "190472", "112574", "191781", "191780", "117303", "132401", "112816", "191882", "191397", "191623", "192029", "111177", "191760", "192441", "192226", "190547", "124464", "24160", "191586", "190513", "191107", "191380", "190616", "125144", "191881", "191879", "191880", "191078", "116142", "191057", "191060", "191065", "112370", "191061", "191965", "190819", "136825", "190693", "190410", "111020", "110955", "190723", "8347", "193188", "190518", "191419", "191587", "191337", "192444", "114359", "190903", "116041", "116096", "190879", "191585", "120032", "124075", "190857", "125065", "132844", "113678", "113679", "191963", "191095", "191890", "1010222", "192013", "24248", "190827", "191695", "112248", "191053", "191694", "191112", "124041", "193190", "112551", "111169", "191578", "101638", "125130", "114541", "1010207", "6280", "124163", "134623", "2226", "191083", "111130", "192468", "190893", "190457", "132310", "132219", "1966", "125124", "125043", "8908", "131387", "124147", "128852", "192326", "191596", "125120", "136649", "191624", "191567", "125146", "114383", "111032", "135130", "191791", "191718", "112388", "191736", "191792", "191856", "24152", "125112", "190737", "190749", "191620", "191766", "112369", "112883", "112887", "112884", "192305", "125069", "190871", "24155", "24153", "8156", "191100", "5395", "116162", "120037", "190869", "190675", "120044", "190455", "192450", "191464", "113800", "24808", "191436", "1010243", "5392", "190630", "191327", "125049", "190870", "190902", "190881", "191838", "191849", "191848", "114401", "190656", "5393", "191682", "191681", "113905", "190759", "120042", "120033", "1010200", "191683", "116173", "192430", "192433", "191505", "1010235", "125115", "191913", "24109", "132092", "190520", "190726", "191570", "191825", "24656", "24368", "190735", "191955", "112365", "112366", "112367", "191601", "191598", "191377", "114538", "190674", "190627", "191465", "191769", "111018", "191756", "191768", "190876", "120033", "114794", "1010208", "1010234", "24837", "270042", "1010247", "190859", "1010209", "191895", "191884", "192360", "190456", "124159", "191313", "191442", "191463", "192030", "190672", "191334", "114861", "114877", "114878", "191811", "192230", "192232", "192233", "191609", "191310", "192208", "136821", "136823", "136819", "136827", "136831", "136835", "136838", "137631", "136833", "124884", "191105", "112612", "192457", "125133", "124161", "190905", "191428", "1010241", "191426", "124870", "191081", "116164", "1010193", "114371", "191062", "112811", "190514", "190519", "1010233", "137281", "1010249", "116190", "190548", "190626", "191733", "112697", "191469", "190451", "1010215", "191564", "190915", "1010192", "1010248", "191374", "192008", "190896", "190925", "120014", "192166", "192197", "191744", "115470", "111107", "191991", "191099", "112381", "192001", "190466", "191689", "124151", "190446", "191590", "116167", "191466", "190746", "112380", "111119", "190680", "114667", "191432", "191597", "112880", "192186", "114795", "113715", "102179", "110942", "110944", "191919", "136712", "191414", "191616", "191626", "191854", "191324", "190748", "190910", "192405", "192222", "191708", "190685", "191342", "191344", "191566", "191727", "191921", "191857", "116166", "191685", "24286", "114865", "114885", "114886", "190540", "190840", "2223", "840", "112815", "114543", "190705", "191317", "24320", "5383", "191581", "191319", "111120", "114860", "114875", "114876", "114364", "114390", "116087", "191743", "191724", "191839", "116186", "124158", "191753", "125046", "113359", "113792", "125063", "114678", "190919", "1010194", "124878", "191063", "190692", "25506", "190725", "191569", "191924", "191927", "112373", "112698", "112703", "190933", "190945", "190942", "190948", "190951", "190939", "190917", "190927", "190921", "191902", "191899", "190924", "190930", "190936", "192036", "191886", "112383", "192185", "5382", "116180", "111019", "104828", "114798", "191092", "190532", "117698", "190625", "124156", "190512", "113472", "124149", "112371", "191732", "1010221", "192002", "111126", "114368", "112368", "125045", "24078", "1414", "190856", "24019", "125127", "110998", "1010190", "191710", "191007", "136739", "111161", "124882", "116089", "116146", "116130", "114796", "114816", "114867", "114868", "190825", "190701", "190818", "190529", "190891", "190533", "190535", "190820", "1010227", "2038", "191621", "191082", "116098", "116083", "190683", "190489", "191090", "191579", "5381", "133183", "112558", "192241", "190453", "114669", "112378", "112379", "112377", "190623", "1010250", "111128", "191354", "191427", "192195", "191573", "191720", "191721", "190473", "116179", "190761", "113194", "125114", "191387", "116088", "131014", "125055", "121020", "124372", "124885", "190741", "124624", "190459", "112390", "191396", "191341", "191104", "111178", "119987", "191698", "111003", "135129", "191615", "190668", "191731", "191728", "191823", "191773", "191329", "192449", "191111", "191625", "113363", "24079", "117699", "190624", "24048", "112566", "191013", "191748", "111002", "191376", "114428", "190639", "190676", "190854", "133185", "191575", "192224", "191308", "191986", "191987", "1010208", "114862", "114879", "114880", "191328", "190617", "125140", "191373", "114391", "114365", "124871", "112818", "124162", "190462", "112561", "191582", "118728", "125136", "535", "2832", "190904", "191782", "191686", "191608", "191737", "191785", "115980", "190724", "190845", "125113", "191793", "104537", "190733", "190493", "191416", "1948", "192356", "136697", "191814", "125128", "191375", "190899", "190898", "113364", "190855", "111047", "111046", "191795", "191813", "191805", "1010236", "191739", "190463", "129237", "129249", "129248", "112933", "113204", "115483", "118634", "113205", "113203", "113192", "113190", "113197", "112937", "115482", "115476", "113188", "129239", "112934", "113198", "112936", "113189", "115478", "112939", "129243", "113202", "113201", "113200", "113199", "115475", "113193", "129222", "129240", "115479", "115481", "113191", "112938", "191408", "191371", "8351", "192460", "191346", "190632", "136647", "114863", "114881", "114882", "113356", "24064", "191584", "112733", "191460", "112563", "191356", "192039", "191430", "190704", "124876", "124877", "191445", "24087", "192331", "191388", "192438", "114662", "192212", "125048", "191413", "190880", "190883", "112879", "190906", "190751", "5397", "112565", "115480", "191888", "101363", "191962", "190848", "190517", "191617", "111190", "111189", "111187", "191790", "190847", "190501", "190500", "190498", "190497", "190499", "112731", "6600", "114673", "190760", "190913", "24081", "191717", "191446", "191572", "1895", "5385", "192484", "191349", "190843", "192231", "190755", "2224", "5398", "191765", "191763", "191762", "111044", "111045", "191794", "191812", "192299", "191796", "191767", "190738", "191395", "190650", "191784", "191778", "191978", "191345", "191386", "190282", "114665", "112549", "191379", "111163", "114657", "191481", "192947", "190505", "125059", "124873", "190621", "114671", "5390", "191459", "124152", "136668", "111167", "191746", "190697", "191749", "190691", "191407", "191799", "191779", "191692", "114402", "125134", "191869", "192168", "191834", "190471", "116160", "191076", "112389", "112553", "116153", "112241", "112242", "112243", "113473", "113360", "5401", "5384", "115273", "116169", "112392", "112393", "190696", "190496", "190475", "125056", "4270", "112520", "191687", "191688", "192229", "191693", "191103", "190720", "116069", "121023", "112568", "112702", "1010566", "112885", "112886", "190824", "190508", "111038", "191822", "191696", "191830", "191307", "192319", "136741", "191309", "191101", "190450", "5386", "112882", "112485", "190907", "111115", "192219", "114666", "136203", "117701", "113358", "124153", "191503", "116148", "116161", "191064", "190826", "191900", "124868", "190481", "133095", "191404", "191398", "191817", "117702", "112735", "191741", "190742", "112700", "190007", "116174", "190629", "191312", "190861", "125123", "191740", "125125", "111034", "125138", "191079", "190618", "1010256", "191467", "190689", "8356", "191080", "112705", "191908", "1010245", "110948", "114542", "5388", "191318", "112611", "190829", "1010017", "113355", "192458", "118594", "110999", "191352", "191697", "190686", "111180", "191054", "5400", "1010199", "112732", "191449", "190962", "114668", "192172", "114799", "116078", "190839", "191897", "120041", "191858", "190744", "190734", "1010246", "104518", "190448", "5394", "191759", "118103", "112247", "120043", "190719", "192155", "114661", "110949", "191619", "190889", "191577", "112144", "191399", "190922", "1010195", "191690", "125054", "190884", "116132", "192361", "191322", "190465", "114674", "190712", "190469", "110938", "111024", "190679", "191355", "190678", "112881", "190756", "102491", "190549", "192184", "24159", "190671", "191089", "190722", "114381", "111116", "124074", "191097", "190487", "24162", "192434", "24164", "190631", "112946", "112951", "112952", "112954", "112955", "112956", "112957", "112958", "112959", "113097", "113098", "113099", "113100", "113101", "113102", "113103", "113104", "113105", "113106", "113109", "113112", "116312", "116313", "116314", "116315", "116316", "116317", "116318", "116319", "116320", "129402", "129403", "129404", "129406", "129407", "129408", "129410", "129411", "129412", "129413", "116042", "192220", "192221", "190932", "190944", "191332", "190941", "190947", "190950", "190938", "190916", "190926", "190920", "191901", "191898", "190923", "190929", "190935", "191589", "191588", "190522", "1010228", "114385", "191801", "125047", "6055", "191909", "112385", "116158", "191896", "191885", "191702", "190467", "136746", "121022", "191400", "24086", "24082", "190447", "191068", "190928", "1010198", "111109", "114859", "114873", "114874", "24108", "24113", "24033", "24036", "190758", "116178", "190550", "536", "119989", "104173", "104175", "104174", "191850", "1010202", "191729", "113353", "191417", "103977", "135086", "190648", "190831", "111123", "112255", "191757", "191084", "191809", "191810", "191368", "1517", "1516", "112245", "191583", "190698", "116175", "111188", "191992", "191993", "102803", "111122", "192012", "190492", "116151", "190646", "190644", "1010257", "190521", "1010017", "191715", "111106", "191750", "190877", "191444", "1010210", "190193", "116071", "191468", "191622", "114358", "1010232", "8355", "190510", "116154", "114357", "190506", "191747", "190732", "1010238", "190666", "190665", "113357", "1010216", "191726", "191565", "191353", "5391", "112518", "112857", "114658", "110983", "114670", "114656", "114410", "190006", "190458", "112696", "191343", "125135", "191789", "191431", "112391", "191006", "191599", "1010206", "192034", "190269", "133181", "116150", "125119", "191066", "114152", "112487", "114155", "114153", "114154", "191580", "192459", "192194", "190695", "191833", "192152", "192153", "192151", "191977", "191832", "191602", "191618", "129251", "190546", "190537", "192227", "24846", "24850", "24851", "190538", "190530", "1010233", "137286", "190541", "190542", "190544", "191102", "190878", "104827", "191851", "191734", "191010", "192443", "192464", "192404", "116147", "112704", "112701", "192463", "125062", "190874", "190873", "191783", "112564", "116177", "112734", "191316", "1010203", "116082", "5803", "191914", "125117", "24012", "125070", "190649", "190484", "190485", "190875", "136648", "191070", "190681", "133532", "125061", "191351", "191730", "124124", "124694", "112523", "191306", "191094", "120030", "191098", "125131", "114408", "114369", "190460", "112550", "117302", "191096", "124154", "191824", "111113", "112384", "192235", "114387", "124879", "190543", "192217", "191803", "191804", "1010225", "191418", "1010224", "190895", "190828", "1010218", "190912", "1010219", "190703", "190503", "191591", "191571", "112382", "192461", "190534", "125051", "136887", "191056", "192225", "192238", "124874", "191855", "111118", "112552", "112372", "1010251", "1010239", "112821", "191738", "192032", "191008", "190864", "113799", "191420", "191920", "192462", "191067", "191055", "191087", "1897", "190480", "190502", "192000", "191772", "191326", "114430", "190474", "116144", "1010234", "124881", "191336", "114404", "190539", "190527", "191405", "192239", "191797", "191798", "190667", "125118", "191372", "191777", "114367", "191840", "125071", "114431", "1956", "190509", "124880", "112483", "125064", "190858", "124883", "190468", "111014", "112728", "112394", "116189", "116189", "112730", "111012", "116187", "190841", "116184", "114677", "132348", "132350", "132351", "132352", "111013", "111004", "112940", "114679", "112941", "111040", "118148", "190885", "111048", "111117", "191722", "113196", "112476", "191714", "116188", "112578", "113195", "192234", "112935", "115477", "191758", "192949", "112272", "192482", "190545", "112375", "116165", "191704", "192359", "192209", "190900", "191815", "190849", "190684", "192177", "114672", "191088", "1010196", "110982", "191716", "192035", "114675", "191402", "190640", "190934", "1010252", "116152", "191735", "798", "114864", "114883", "114884", "114866", "114887", "114888", "190642", "191474", "120035", "191385", "114676", "192364", "115472", "135231", "192007", "128985", "190853", "190736", "24043", "190682", "113362", "118122", "191770", "190688", "24187", "112386", "1961", "192164", "112559", "191448", "114389", "125141", "192948", "124148", "190851", "191829", "135230", "112376", "116099", "116043", "116070", "192415", "112817", "190739", "115473", "8348", "190470", "190507", "136202", "191109", "112280", "129442", "190747", "191415", "190757", "190677", "190653", "190619", "190867", "191603", "191604", "191605", "191606", "192363", "190643", "190852", "190753", "135233", "116181", "190866", "190652", "191677", "191764", "128690", "125129", "116040", "132840", "114356", "191745", "24016", "25073", "191320", "116176", "24034", "112736", "111111", "24654", "24373", "190651", "24305", "1010221", "191325", "190700", "112699", "191412", "191878", "193191", "190452", "190552", "192357", "112569", "111131", "111129", "124150", "190743", "124155", "4135", "192200", "192201", "192203", "192202", "191607", "136886", "1010207", "190754", "125145", "115474", "190862", "1010242", "120008", "116100", "191894", "190911", "191883", "190494", "190524", "1010197", "1010237", "120043", "24154", "110936", "190702", "125060", "114858", "114871", "114872", "111110", "190750", "116163", "1010229", "1010250" };

void Menu::Render(bool* p_open)
{
	// Render the menu
	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = true;
	static bool no_close = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;
	static bool unsaved_document = false;
	static bool auto_resize = true;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (auto_resize)		window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	if (no_close)           p_open = NULL; // Don't pass our bool* to Begin

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 4.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(1.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Right;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 4.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 2.0f);
	style.FrameRounding = 4.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(10.0f, 2.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 12.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.9176470637321472f, 0.9176470637321472f, 0.9176470637321472f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.85f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1098039224743843f, 0.1098039224743843f, 0.1098039224743843f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1098039224743843f, 0.1098039224743843f, 0.1098039224743843f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.5299999713897705f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2078431397676468f, 0.2078431397676468f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.4666666686534882f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8078431487083435f, 0.8274509906768799f, 0.8078431487083435f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.929411768913269f, 0.6470588445663452f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.2078431397676468f, 0.2078431397676468f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2078431397676468f, 0.2078431397676468f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.5098039507865906f, 0.3568627536296844f, 0.1490196138620377f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.7764706015586853f, 0.5490196347236633f, 0.2078431397676468f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.09803921729326248f, 0.1490196138620377f, 0.9700000286102295f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1372549086809158f, 0.2588235437870026f, 0.4196078479290009f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);

	// We specify a default position/size in case there's no data in the .ini file.
	// We only do it to make the demo applications a little more welcoming, but typically this isn't required.
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Anno 1800 Mod Menu v1.5", p_open, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.65f);
		ImGui::PushItemWidth(-140);

		if (ImGui::CollapsingHeader("CHEATS", 32))
		{
			if (ImGui::TreeNode("Common"))
			{
				if (ImGui::Button("Toggle##1")) { sendCommand("ts.Cheat.GlobalCheats.ToggleIgnoreFertilities()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle ignore fertilities");

				if (ImGui::Button("Toggle##2")) { sendCommand("ts.Cheat.GlobalCheats.ToggleSuperShipSpeed()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle super ship speed");

				if (ImGui::Button("Toggle##3")) { sendCommand("ts.Cheat.GlobalCheats.ToggleElectricity()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle electricity");

				if (ImGui::Button("Toggle##4")) { sendCommand("ts.Cheat.GlobalCheats.ToggleIgnoreBuildingCosts()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle ignore building costs");

				if (ImGui::Button("Toggle##5")) { sendCommand("ts.Cheat.GlobalCheats.ToggleProductivity()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle productivity");

				if (ImGui::Button("Toggle##6")) { sendCommand("ts.Participants.CheatTotalTrade()"); }
				ImGui::SameLine(90);
				ImGui::Text("Total trade");

				if (ImGui::Button("Toggle##7")) { sendCommand("ts.Participants.CheatTotalWar()"); }
				ImGui::SameLine(90);
				ImGui::Text("Total war");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Items"))
			{
				static int i0 = 100000;
				if (ImGui::Button("Add##1")) { sendCommand("ts.Area.Current.Economy.AddAmount(1010017, " + std::to_string(i0) + ")"); }
				ImGui::SameLine(90);
				ImGui::PushItemWidth(125);
				ImGui::InputInt(" coins to current economy##inputi0", &i0);

				static int i1 = 100;
				if (ImGui::Button("Add##2")) { sendCommand("ts.Area.Current.Economy.AddAmount(" + std::to_string(i1) + ")"); }
				ImGui::SameLine(90);
				ImGui::PushItemWidth(125);
				ImGui::InputInt(" goods to current economy##inputi1", &i1);

				static int i2 = 0;
				if (ImGui::Button("Add##3")) { std::string guid = (guids[i2]); sendCommand("ts.Selection.Object.ItemContainer.SetEquipSlot(0, -1)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(" + guid + ", 1)"); }
				ImGui::SameLine(90);
				ImGui::PushItemWidth(125);
				ImGui::Combo(" (GUID) to current selected ship##inputi2", &i2, items, IM_ARRAYSIZE(items), 20);

				if (ImGui::Button("Add##4")) { sendCommand("ts.Selection.Object.ItemContainer.SetEquipSlot(0, 0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(0)\r\nts.Selection.Object.ItemContainer.SetClearSlot(1)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010196, 50)\r\nts.Selection.Object.ItemContainer.SetCheatItemInSlot(1010218, 50)"); }
				ImGui::SameLine(90);
				ImGui::Text("Add building materials to current selected ship");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Discovery"))
			{
				if (ImGui::Button("Toggle##9")) { sendCommand("TextSources.TextSourceRoots.Discovery.ShowAll()"); }
				ImGui::SameLine(90);
				ImGui::Text("Discover whole map");

				if (ImGui::Button("Toggle##10")) { sendCommand("TextSources.TextSourceRoots.Discovery.HideAll()"); }
				ImGui::SameLine(90);
				ImGui::Text("Hide whole map");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Weather"))
			{
				if (ImGui::Button("Toggle##11")) { sendCommand("ts.Weather.SetChangeWind()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle wind direction");

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Incidents"))
			{
				if (ImGui::Button("Toggle##12")) { sendCommand("ts.Cheat.GlobalCheats.ToggleIncidents()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle incidents");

				if (ImGui::Button("Toggle##13")) { sendCommand("ts.Incidents.ToggleSpreading()"); }
				ImGui::SameLine(90);
				ImGui::Text("Toggle incident spreading");

				if (ImGui::Button("Toggle##14")) { sendCommand("ts.Incidents.CheatUnlockAllIncidents()"); }
				ImGui::SameLine(90);
				ImGui::Text("Unlock all incidents");

				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("MODDING TOOLS", 32))
		{

			if (ImGui::Button("Show/Hide Console")) { sendCommand("console.toggleVisibility()"); }

			if (ImGui::Button("Show/Hide File Editor")) {
				if (showModEditor) {
					showModEditor = false;
				}
				else {
					showModEditor = true;
				}
			}
		}

		ImGui::Text(" ");
		ImGui::Text("By ChrisAnd1998");
		ImGui::Text("NOTE: Show/Hide this menu with F7 key.");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		isWindowHovered = ImGui::IsWindowHovered(0);
		isItemHovered = ImGui::IsAnyItemHovered();

		ImGui::End();


		///////////////////////////////////////////////////////////////////////////////// Start Mod Editor
		if (showModEditor) {
			
			ImGui::Begin("File Editor", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
			ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("Open")) {
						//openfile = true;
						fileDialog.Open();
					}

					if (ImGui::MenuItem("Save"))
					{
						savefile = true;
					}

					if (ImGui::MenuItem("Quit", "Alt-F4"))
						showModEditor = false;

					ImGui::EndMenu();
					
				}
				if (ImGui::BeginMenu("Edit"))
				{
					bool ro = editor.IsReadOnly();
					if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
						editor.SetReadOnly(ro);
					ImGui::Separator();

					if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
						editor.Undo();
					if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
						editor.Redo();

					ImGui::Separator();

					if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
						editor.Copy();
					if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
						editor.Cut();
					if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
						editor.Delete();
					if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
						editor.Paste();

					ImGui::Separator();

					if (ImGui::MenuItem("Select all", nullptr, nullptr))
						editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Palette"))
				{
					if (ImGui::MenuItem("Dark palette"))
						editor.SetPalette(TextEditor::GetDarkPalette());
					if (ImGui::MenuItem("Dark palette"))
						editor.SetPalette(TextEditor::GetMarianaPalette());
					if (ImGui::MenuItem("Light palette"))
						editor.SetPalette(TextEditor::GetLightPalette());
					if (ImGui::MenuItem("Retro blue palette"))
						editor.SetPalette(TextEditor::GetRetroBluePalette());
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Language"))
				{
					//if (ImGui::MenuItem("XML"))
						//editor.SetLanguageDefinition(TextEditor::LanguageDefinition::XML());
					if (ImGui::MenuItem("C++"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
					if (ImGui::MenuItem("HLSL"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::HLSL());
					if (ImGui::MenuItem("GLSL"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
					if (ImGui::MenuItem("Python"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Python());
					if (ImGui::MenuItem("C"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::C());
					if (ImGui::MenuItem("SQL"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::SQL());
					if (ImGui::MenuItem("Angel Script"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::AngelScript());
					if (ImGui::MenuItem("Lua"))
						editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
					ImGui::EndMenu();


					


				}

				ImGui::EndMenuBar();
			}


			if (currentFile == "") {
				ImGui::Text("No file selected. [File] -> [Open]");
			}
			else {
				ImGui::Text(currentFile.c_str());
			}

			//ImGui::Text(currentFile.c_str());

			//editor.GetLanguageDefinitionName();
			

			editor.Render("TextEditor");

			

			ImGui::End();

			
		}
		//////////////////////////////////////////////////////////////////////////////////// End Mod Editor

		///////////////////////////////////////////////////////////////////////////////// Start Open File
		fileDialog.SetTitle("Open File");
		fileDialog.SetTypeFilters({ ".xml", ".lua", ".py" });

		fileDialog.Display();

		if (fileDialog.HasSelected())
		{
			//MessageBox(NULL, fileDialog.GetSelected().string().c_str(), "", MB_OK);
			//fileEdit(fileDialog.GetSelected().string());	
			currentFile = fileDialog.GetSelected().string();
			fileEdit(currentFile);
			editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
			fileDialog.ClearSelected();
		}
	
		if (savefile) {
			//MessageBox(NULL, currentFile.c_str(), "", MB_OK);
			auto textToSave = editor.GetText();
			std::ofstream outfile(currentFile);
			outfile << textToSave << std::endl;
			outfile.close();
			savefile = false;
		}
		///////////////////////////////////////////////////////////////////////////////// End Open File

		ImGui::Render();
}