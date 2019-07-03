#include<bits/stdc++.h>
using namespace std;

int expendNode = 0;
//int ranNode = 0;
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

void loadDomain(vector < vector <string> > &domain, vector < vector <int> > &position, vector <string> &dictionary, 
	vector <bool> visited, int depth){
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

void loadMap2(vector < vector <int> > &position, vector < vector <char> > &map, int depth){
	for(int i=0; i<depth; i++){
		if(position[i][3] == 0){
			for(int j=0; j<position[i][2]; j++){
				map[position[i][1]][position[i][0]+j]++;
			}
		}else{
			for(int j=0; j<position[i][2]; j++){
				map[position[i][1]+j][position[i][0]]++;
			}
		}
	}
} // set the map for binaryConstraint to use, '2' means there has binaryConstraint

int findMin(vector < vector <int> > &position, vector < int > &domainSize, vector < vector <char> > &map,
 vector <bool> visited,  vector < int > &binaryCons, int depth){
	int min = 3001;
	int index = 0;
	for(int i=0; i<depth; i++){
		if(min > domainSize[i] && !visited[i]){
			min = domainSize[i];
			index = i;
		}else if(min == domainSize[i] && !visited[i]){ // if domain size equal, return the one which has more binaryContraint
			if(binaryCons[index] > binaryCons[i]){
				continue;
			}else{
				index = i;
			}
		}
	}
	return index;
}	// find the minimum domain for the current domain, return that variable 

void loadBinaryCons(vector < vector <int> > &position, vector < vector <char> > &map, vector < int > &binaryCons,
 int depth){
	for(int i=0; i<depth; i++){
		if(position[i][3] == 0){
			for(int j=0; j<position[i][2]; j++){
				if(map[position[i][1]][position[i][0]+j] == '2'){
					binaryCons[i]++;
				}
			}
		}else{
			for(int j=0; j<position[i][2]; j++){
				if(map[position[i][1]+j][position[i][0]] == '2'){
					binaryCons[i]++;
				}
			}
		}
	}
} // set how many binaryConstraint that each variable has, "degree heuristic"

void setConstraint(vector < vector <int> > &position, vector < vector <string> > &domain, vector < int > domainSize,
 vector < vector <char> > &map, vector <bool> visited, int depth){
	for(int i=0; i<depth; i++){
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
} // update the domain for each variable by current map

void dfsSearch(vector < vector <int> > &position, vector < vector <string> > domain, stack < node > &explored,
 stack < node > &frontier, vector < vector <char> > &map, vector < int > domainSize, vector <bool> visited, 
  vector < int > &binaryCons, vector <string> &dictionary, int depth, int newNode){
	if(explored.size() == depth) return;
	int index;
	index = findMin(position, domainSize, map, visited, binaryCons, depth);
	// the variable that we want to fill, "MRV heuristic"
	
	if(domain[index].size() == 0){
		int undo = explored.top().variable;
		visited[undo] = false;
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
	// if that word will let the remain blank can't fill word, pop it and use another frontier = "forward checking"
	// and draw a new map by current explored

	visited[index] = true;
	// use for dfs to remember which variable has ran

	newNode = 0;
	// use to memorize how many node will expend if this current node push

	for(int i=domainSize[index]-1; i>=0; i--){
		node temp;
		temp.variable = index;
		temp.value = domain[index][i];
		frontier.push(temp);
		newNode++;
		expendNode++;
	}
	// expend node

	while(!frontier.empty() && explored.size()!=depth){
		if(newNode == 0) {
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
		// newNode == 0 means the frontier it expend is all pop, i need a new explore and return to it's domain

		int draw = frontier.top().variable;
		if(position[draw][3] == 0){
			for(int j=0; j<position[draw][2]; j++){
				map[position[draw][1]][position[draw][0]+j] = frontier.top().value[j];
			}
		}else{
			for(int j=0; j<position[draw][2]; j++){
				map[position[draw][1]+j][position[draw][0]] = frontier.top().value[j];
			}
		}	
		//draw the word on the map
		
		vector < vector <string> > newDomain(depth);
		vector < int > newDomainSize(depth);
		// new domain and domainsize for dfs

		for(int i=0; i<depth; i++){
			newDomain[i] = domain[i];
		}
		newDomainSize = domainSize;


		setConstraint(position, newDomain, newDomainSize, map, visited, depth);	
		//rebuilt each domain
		
		makeSize(newDomain, newDomainSize, depth);	//rebuilt each size
		
		explored.push(frontier.top());
		ranNode++;	
		//let the word in the explored
		
		frontier.pop();	
		//let the word out of frontier
		newNode--;

		dfsSearch(position, newDomain, explored, frontier, map, newDomainSize, visited, binaryCons, dictionary,
		 depth, newNode);
	}
}

int main(){
	vector < vector <int> > position;	
	//to store puzzle.txt's information: x, y, length, direction
	
	vector <string> dictionary;	
	//to store 3000 english word
	
	stack < node > explored;	
	//data structure to store the node we explored, in detail, which blank we fill in what word
	
	stack < node > frontier;	
	//data structure to store the node we can explored, in detail, which will be the next to refill

	ifstream fin("puzzle.txt", ios::in);
	string str;

	loadWord(dictionary);	
	// read 3000 english word

	getline(fin,str);
	getline(fin,str);
	getline(fin,str);
	getline(fin,str); 
	// first test for one getline (i don't have time to built it in while, it will explode QQ)

	position.clear();
	if(!explored.empty()){
		explored.pop();
	}
	if(!frontier.empty()){
		frontier.pop();
	}	
	// initialize

	vector < vector <char> > map(8, vector<char> (8, '0'));	
	//to store current drawing on the map, '1' for blank, '0' can't fill
	
	vector < vector <char> > map2(8, vector<char> (8, '0')); 
	// map to find binaryconstraint
	
	int i=0;
	int depth=0;	
	//how many variable to fill
	istringstream iss(str);
	vector <int> tmp;
	char temp;
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
	}	
	// memorize the position for each variable
	
	int newNode=0;

	vector <bool> visited(depth, false);	
	// to recgonize which variable have filled
	
	vector < vector <string> > domain(depth);	
	// to memorize each variable's domain by length
	
	loadDomain(domain, position, dictionary, visited, depth);	
	// built domain by length
	
	vector < int > domainSize(depth, 0);	
	// each variable's domain size
	
	vector < int > binaryCons(depth, 0);
	// each variable's binaryconstraint's number

	makeSize(domain, domainSize, depth);	
	// make the size of each domain
	
	loadMap(position, map, depth);	
	// built the initial map
	
	loadMap2(position, map2, depth);
	// built the map for binaryconstraint

	loadBinaryCons(position, map2, binaryCons, depth);
	// count binaryConstraint

  dfsSearch(position, domain, explored, frontier, map, domainSize, visited, binaryCons, dictionary, depth, newNode);
  // main algorithm

	

  cout << "final:" << endl;
	for(int i=0; i<8; i++){
			for(int j=0; j<8; j++){
				cout << map[i][j] << " ";
			}
		cout << endl;
	}	
	cout << endl;
	cout << "f: " << frontier.size() << "e: " << explored.size() << endl;
	cout << "expendNode: " << expendNode << endl;
	//cout << "ranNode: " << ranNode << endl;
	// final answer

	return 0;
}
