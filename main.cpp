#include <set>
#include <cstdlib>
#include <fstream>
#include <iostream>

using namespace std;

// --------------- global vars

string currDir = "# ";
fstream dir;
char memDir[1024][33];

// --------------- functions declarations

void createhd(string input);
void openDir(string input);
void rootDir();
void create(string input);
void createHDFile();
void saveFile(string filename,string fileContent);
int nextHDSlot();
int nextLine();
void saveMemDir();
void removeFile(string input);
void removeFileContent(int contentIndex);

// ---------------

int main() {

	string input;
	while(input != "exit"){
		cout << currDir;
		getline(cin, input);

		if(input.find("createhd ") == 0){
			createhd(input);
		} else if(input.find("create ") == 0){
			create(input);
		} else if(input.find("cd ..") == 0){
			rootDir();
		} else if(input.find("selecthd ") == 0){
			openDir(input);
		} else if(input.find("remove ") == 0){
            removeFile(input);
        }
	}
}

void createhd(string input){

	string name = input.replace(0, 9, "");
    currDir = "# " + name + ":\\> ";
    string temp = "fsutil file createnew " + name + ".txt 33792";
    system(temp.c_str());

	temp = name + ".txt";
    dir.open(temp.c_str());

    createHDFile();
}

void openDir(string input){
    string name = input.replace(0, 8, "");
    string temp = name + ".txt";
    currDir = "# " + name + ":\\> ";

	dir.open(temp.c_str());
}

void create(string input){

	input = input.replace(0, 7, "");
	
	string fileCont;
	
	getline(cin, fileCont);

    saveFile(input, fileCont);
}

void createHDFile(){
	int i,j;

	for(i = 0; i < 1024; i++){
		for(j = 0; j < 33; j++){
			if(j== 0){
                memDir[i][j] = '0';
            } else if(j == 32){
                memDir[i][j] = '\n';
            } else if(i <= 19 && j == 1){
                memDir[i][j] = '0';
            } else if(i >= 20 && j == 1){
                memDir[i][j] = '1';
            } else {
                memDir[i][j] = ' ';
            }
            dir<<memDir[i][j];
		}
	}
}

void rootDir(){
	dir.close();
	currDir = "# ";
}

void saveFile(string filename, string fileContent){

    int freeHDSlot = nextHDSlot();
    
    int contIndex = nextLine();
    
    int fileContSize = fileContent.size();
    int aux = 0;

    // <--- writing dir block --->
    memDir[freeHDSlot][0] = '1';

	memDir[freeHDSlot][2] = '0';

    for(int j = 4; j < 8; j++){
        memDir[freeHDSlot][j] = '0'; // might change later
    }

    string sHDIndex = to_string(contIndex);
                        
    int sHDSize = sHDIndex.size();

    for(int j = 8 + (4 - sHDSize); j < 12; j++){
    	if( aux < sHDSize){
    		memDir[freeHDSlot][j] = sHDIndex.at(aux);
    		aux++;
		} else {
			break;
		}
    }
    
    for(int j = 11 - sHDSize; j >= 8 ; j--){
    	memDir[freeHDSlot][j] = '0';
	}

    // ------------------

    sHDIndex = to_string(fileContSize);
                        
    sHDSize = sHDIndex.size(); 
    
    aux = 0;
    
    for(int j = 12 + (4 - sHDSize); j < 16; j++){
    	if( aux < sHDSize){
    		memDir[freeHDSlot][j] = sHDIndex.at(aux);
    		aux++;
		} else {
			break;
		}
    }
    
    for(int j = 15 - sHDSize; j >= 12 ; j--){
    	memDir[freeHDSlot][j] = '0';
	}

    int fNameSize = filename.size();

    aux = 0;

    for(int j = 16; j < 32; j++){
        if(aux < fNameSize){
            memDir[freeHDSlot][j] = filename.at(aux);
            aux++;
        }
    }

    // <--- handling file logic/block --->

    aux = 0;

    for(int i = 0; i < (fileContent.size()/24)+1; i++){
    	    	
		memDir[contIndex][0] = '1';

        if(i == (fileContent.size()/23)){
            for(int k = 4; k < 8; k++){
                memDir[contIndex][k] = '0';
            }
        } else {
            string sIndex = to_string(contIndex+1);
                        
            int sSize = sIndex.size();
            
            int temp = 0;

            for(int k = 4 + (4 - sSize); k < 8; k++){
            	if( temp < sSize){
            		memDir[contIndex][k] = sIndex.at(temp);
            		temp++;
				} else {
					break;
				}
            }
            
            for(int k = 7 - sSize; k >= 4 ; k--){
            	memDir[contIndex][k] = '0';
			}
        }

        for(int j = 8; j<32;j++){
        	if (aux < fileContSize){
        		memDir[contIndex][j] = fileContent.at(aux);
        		aux++;
			} else {
				break;
			}
        }
        contIndex = nextLine();
    }

    saveMemDir();

}

void saveMemDir(){
	
	dir.clear();
	dir.seekg(0, ios::beg);
	
	for(int i = 0; i < 1024; i++){
		for(int j = 0; j < 33; j++){
            dir<<memDir[i][j]; 
		}
	}
}

int nextHDSlot(){
	
	int freeHDSlot;
	
	for(int i = 0; i < 20; i++ ){
        if(memDir[i][0] == '0'){
            freeHDSlot = i;
            break;
        }
    }
    
    return freeHDSlot;
}

int nextLine(){
    int index;
    for(int i = 20; i < 1024; i++ ){
        if(memDir[i][0] == '0'){
            index = i;
            break;
        }
    }

    return index;
}

void removeFile(string input){

    bool fileExists = false;

    string fileName = input.replace(0, 7, "");

    int lineIndex;

    for(int i = 0; i < 20; i++){
        cout << "memDir[" << i << "][0] => " << memDir[i][0] << " | ";
        cout << "memDir[" << i << "][2] => " << memDir[i][2] << '\n';
        if(memDir[i][0] == '1' && memDir[i][2] == '0'){
            
            string aux;

            int j = 16;

            while(memDir[i][j] != ' ' && j < 32){

                aux.push_back(memDir[i][j]);

                j++;

            }

            if(aux.size() > 0){
                if(fileName.compare(aux) == 0){

                    memDir[i][0] = '0';

                    fileExists = true;

                    aux.clear();

                    for(j = 8; j < 12; j++){
                        aux.push_back(memDir[i][j]);
                    }

                    cout << "fileName => " << fileName << " | ";
                    cout << "aux -> " << aux << '\n';

                    lineIndex = stoi(aux);

                    break;
                }
            }
        }
    }

    if(fileExists){
        cout<<"calling removeFileContent(lineIndex)"<<'\n';
        removeFileContent(lineIndex);
    } else {
        cout << "O arquivo nao foi encontrado!" << '\n';
    }
}

void removeFileContent(int contentIndex){
        
    if(contentIndex == 0){

        memDir[contentIndex][0] = '0';

    } else {

        int nextLineIndex;

        string aux;

        memDir[contentIndex][0] = '0';

        aux.clear();

        for(int j = 4; j < 8; j++){
            aux.push_back(memDir[contentIndex][j]);
        }

        nextLineIndex = stoi(aux);

        removeFileContent(nextLineIndex);
    }
}