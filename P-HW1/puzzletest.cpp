#include<bits/stdc++.h>
using namespace std;

int expendNode = 0;
int ranNode = 0;
struct node{
	int variable;	// to store which blank we filled
	string value;	// to store what word we filled
};

void loadWord(vector< string > &dictionary){
	ifstream fliein("English words 3000.txt", ios::in);
	string word;
	while(getline(fliein, word)){
		istringstream is(word);
		string tempword;
		is >> tempword;
		dictionary.push_back(tempword);
	}
}	// memorize 3000 english word

void loadDomain(vector < vector <string> > &domain, vector < vector <int> > &position, vector <string> &dictionary, vector <bool> visited, int depth){
	for(int i=0; i<depth; i++){
		if(!visited[i]){
			for(int j=0; j<2932; j++){
				if(position[i][2] == dictionary[j].length()){
					domain[i].push_back(dictionary[j]);
				}
			}
		}		
	}
}	// find the initial domain for each variable by length

void makeSize(vector < vector <string> > &domain, vector <int > &domainSize, int depth){
	for(int i=0; i<depth; i++){
		domainSize[i] = domain[i].size();
	}
}	// find the domain size for each variable

void loadMap(vector < vector <int> > &position, vector < vector <char> > &map, int depth){
	for(int i=0; i<depth; i++){
		if(position[i][3] == 0){
			for(int j=0; j<position[i][2]; j++){
				map[position[i][1]][position[i][0]+j] = '1';
			}
		}else{
			for(int j=0; j<position[i][2]; j++){
				map[position[i][1]+j][position[i][0]] = '1';
			}
		}
	}
}	// set the initial map, '1' for blank that can fill word, '0' for the place can't fill word

int findMin(vector < vector <int> > &position, vector < int > &domainSize, vector < vector <char> > &map, vector <bool> visited, int depth){
	int min = 3001;
	int index = 0;
	for(int i=0; i<depth; i++){
		if(min > domainSize[i] && !visited[i]){
			min = domainSize[i];
			index = i;
		}else if(min == domainSize[i] && !visited[i]){
			int indexDgree = 0;
			int iDegree = 0;
			if(position[i][3] == 0){
				for(int j=0; j<position[i][2]; j++){
					if(map[position[i][1]][position[i][0]+j] != '1'){
						iDegree++;
					}
				}
			}else{
				for(int j=0; j<position[i][2]; j++){
					if(map[position[i][1]+j][position[i][0]] != '1'){
						iDegree++;
					}
				}
			}
			if(position[index][3] == 0){
				for(int j=0; j<position[index][2]; j++){
					if(map[position[index][1]][position[index][0]+j] != '1'){
						indexDgree++;
					}
				}
			}else{
				for(int j=0; j<position[i][2]; j++){
					if(map[position[index][1]+j][position[index][0]] != '1'){
						indexDgree++;
					}
				}
			}
			if(iDegree < indexDgree){
				min = domainSize[i];
				index = i;
			}else{
				min = domainSize[index];
				index = index;
			}
		}
	}

	return index;
}	// find the minimum domain for the current domain, return that variable 

void setConstraint(vector < vector <int> > &position, vector < vector <string> > &domain, vector < int > domainSize, vector < vector <char> > &map, vector <bool> visited, int depth){
	//cout << "before set cons" << endl;
	//for(int i=0; i<8; i++){
	//		for(int j=0; j<8; j++){
	//			cout << map[i][j] << " ";
	//		}
	//	cout << endl;
	//}
	for(int i=0; i<depth; i++){
		//cout << domainSize[i] << " ";
		if(!visited[i]){
			if(position[i][3] == 0){
				for(int j=0; j<position[i][2]; j++){
					if(map[position[i][1]][position[i][0]+j] != '1'){
						for(int k=0, c=0; c<domainSize[i]; k++,c++){
							if(domain[i][k][j] != map[position[i][1]][position[i][0]+j]){
								domain[i].erase(domain[i].begin()+k);
								k--;
							}
						}	
					}
				}
			}else{
				for(int j=0; j<position[i][2]; j++){
					int t = 0;
					if(map[position[i][1]+j][position[i][0]] != '1'){
						for(int k=0, c=0; c<domainSize[i]; k++,c++){
							if(domain[i][k][j] != map[position[i][1]+j][position[i][0]]){
								domain[i].erase(domain[i].begin()+k);
								k--;
							}
						}
					}
				}
			}	
		}
	}
	//cout << endl;
	//for(int i=0; i<8; i++){
	//		for(int j=0; j<8; j++){
		//		cout << map[i][j] << " ";
	//		}
	//	cout << endl;
	//}
}

void dfsSearch(vector < vector <int> > &position, vector < vector <string> > domain, stack < node > &explored, stack < node > &frontier, vector < vector <char> > &map, vector < int > domainSize, vector <bool> visited, vector <string> &dictionary, int depth, int newNode){
	if(explored.size() == depth) return;
	int index;
	for(int i=0; i<depth; i++){
		if(!visited[i]){
			index = i;
			break;
		}
	}
	//index = findMin(position, domainSize, map, visited, depth);

	//cout << "now: " << index << endl; 
	//for(int i=0; i<domain[index].size(); i++){
	//	cout << domain[index][i] << " ";
	//}
	//cout << endl;
	if(domain[index].size() == 0){
		//cout << "nothing can fill" << endl;
		int undo = explored.top().variable;
		visited[undo] = false;
		//cout << explored.top().value << endl;
		explored.pop();
		loadMap(position, map, depth);
		stack < node > temp = explored;
		while(!temp.empty()){
			int tempindex = temp.top().variable;
			if(position[tempindex][3] == 0){
				for(int j=0; j<position[tempindex][2]; j++){
					map[position[tempindex][1]][position[tempindex][0]+j] = temp.top().value[j];
				}
			}else{
				for(int j=0; j<position[tempindex][2]; j++){
					map[position[tempindex][1]+j][position[tempindex][0]] = temp.top().value[j];
				}
			}
			temp.pop();
		}
		return;	//if that word will let the remain blank can't fill word, pop it and use another frontier
	}
	visited[index] = true;
	newNode = 0;
	for(int i=domainSize[index]-1; i>=0; i--){
		node temp;
		temp.variable = index;
		temp.value = domain[index][i];
		frontier.push(temp);
		newNode++;
		expendNode++;
	}
	while(!frontier.empty() && explored.size()!=depth){
		if(newNode == 0) {
			//cout << "newNode == 0, pop " << explored.top().value << endl;
			explored.pop();
			loadMap(position, map, depth);
			stack < node > temp = explored;
			while(!temp.empty()){
				int tempindex = temp.top().variable;
				if(position[tempindex][3] == 0){
					for(int j=0; j<position[tempindex][2]; j++){
						map[position[tempindex][1]][position[tempindex][0]+j] = temp.top().value[j];
					}
				}else{
					for(int j=0; j<position[tempindex][2]; j++){
						map[position[tempindex][1]+j][position[tempindex][0]] = temp.top().value[j];
					}
				}
				temp.pop();
			}
			return;
		}
		int draw = frontier.top().variable;
		if(position[draw][3] == 0){
			for(int j=0; j<position[draw][2]; j++){
				map[position[draw][1]][position[draw][0]+j] = frontier.top().value[j];
			}
		}else{
			for(int j=0; j<position[draw][2]; j++){
				map[position[draw][1]+j][position[draw][0]] = frontier.top().value[j];
			}
		}	//draw the word on the map
		//loadDomain(domain, position, dictionary, visited, depth);
		vector < vector <string> > newDomain(depth);
		vector < int > newDomainSize(depth);
		for(int i=0; i<depth; i++){
			newDomain[i] = domain[i];
		}
		newDomainSize = domainSize;

		//cout << frontier.top().value << endl;
		//if(!explored.empty())cout << explored.top().value << endl;

		setConstraint(position, newDomain, newDomainSize, map, visited, depth);	//rebulit each domain
		makeSize(newDomain, newDomainSize, depth);	//rebuilt each size
	
		explored.push(frontier.top());	//let the word in the explored
		ranNode++;
		frontier.pop();	//let the word out of frontier
		newNode--;

		//for(int i=0; i<depth; i++){
		//	cout << newDomainSize[i] << " ";
		//}
		//cout << endl;

		//cout << "f: " << frontier.size() << "e: " << explored.size() << endl;

		//cout << endl;
		dfsSearch(position, newDomain, explored, frontier, map, newDomainSize, visited, dictionary, depth, newNode);
	}
}

int main(){
	vector < vector <int> > position;	//to store puzzle.txt's information: x, y, length, direction
	vector <string> dictionary;	//to store 3000 english word
	stack < node > explored;	//data structure to store the node we explored, in detail, which blank we fill in what word
	stack < node > frontier;	//data structure to store the node we can explored, in detail, which will be the next to refill

	ifstream fin("puzzle.txt", ios::in);
	string str;

	loadWord(dictionary);	// read 3000 english word

	//while(getline(fin,str)){	//main code to do search, one line one output
	getline(fin,str);
	getline(fin,str);
	getline(fin,str);
	getline(fin,str);
		position.clear();
		if(!explored.empty()){
			explored.pop();
		}
		if(!frontier.empty()){
			frontier.pop();
		}	// initialize

		vector < vector <char> > map(8, vector<char> (8, '0'));	//to store current drawing on the map, 'x' for blank, '0' can't fill
		istringstream iss(str);
		vector <int> tmp;
		char temp;
		int i=0;
		int depth=0;	//how many variable to fill
		int newNode=0;
		while(iss >> temp){
			if(i<4){
				if(temp == 'A'){
					tmp.push_back(0);
					i++;
				}else if(temp == 'D'){
					tmp.push_back(1);
					i++;
				}else{
					tmp.push_back(temp-'0');
					i++;
				}
			}
			if(i==4){
				position.push_back(tmp);
				tmp.clear();
				i = 0;
				depth++;
			}
		}	// position build finished

		vector <bool> visited(depth, false);	// to recgonize which variable have filled
		vector < vector <string> > domain(depth);	// to memorize each variable's domain by length
		//domain.clear();
		//for(int i=0; i<depth; i++){
		//	domain[i].clear();
		//}
		loadDomain(domain, position, dictionary, visited, depth);	// built domain by length
		vector < int > domainSize(depth, 0);	// each variable's domain size
		makeSize(domain, domainSize, depth);	// make the size of each domain
		loadMap(position, map, depth);	// built the initial map

		/*for(int i=0; i<3000; i++){
			cout << dictionary[i] << endl;
		}*/
		/*for(int i=0; i<depth; i++){
				for(int j=0; j<domainSize[i]; j++){
					cout << domain[i][j] << " ";
				}
			cout << endl;
		}*/
	  dfsSearch(position, domain, explored, frontier, map, domainSize, visited, dictionary, depth, newNode);

		/*for(int i=0; i<depth; i++){
			cout << domainSize[i] << " ";
		}*/

	  cout << "final:" << endl;
		for(int i=0; i<8; i++){
				for(int j=0; j<8; j++){
					cout << map[i][j] << " ";
				}
			cout << endl;
		}	// code to show map
		cout << endl;
		cout << "f: " << frontier.size() << "e: " << explored.size() << endl;
		cout << "expendNode: " << expendNode << endl;
		cout << "ranNode: " << ranNode << endl;
	//}
	return 0;
}
