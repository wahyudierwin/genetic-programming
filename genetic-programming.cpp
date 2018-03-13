#include <bits/stdc++.h>
#include <random>

using namespace std;

double EPS = 1e-9;

struct node
{
	//kind of type : 0 = variable, 1 = const, 2 = operator
	//if type = 1, so value = its value
	//if type = 2, so value = the operator
	// value = 0 => multiply
	// value = 1 => divide
	// value = 2 => add
	// value = 3 => substract
	int type;
	double value;
	int numSubTree; //number of nodes where this is the root
	node* leftChild;
	node* rightChild;

	node()
	{
		leftChild = NULL;
		rightChild = NULL;
		value = 0.0;
		numSubTree = 1;
	}
};

double fRand(double fMin, double fMax)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<> dis(fMin, fMax+EPS);

	double t = dis(gen);
	return min(t, fMax);
}

int iRand(int iMin, int iMax)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(iMin, iMax);

	int t = dis(gen);
	return t;
}

int numPopulation = 10; //number of chromosomes in population
int numIteration = 3; //number of iteration
int numInput;

vector<node*> population;
vector<double> populationFitness;

vector<double> xValues;
vector<double> yValues;

node* copyTree(node* a)
{
	node* newNode;
	if (a != NULL)
	{
		newNode = new node;
		newNode->type = a->type;
		newNode->value = a->value;
		newNode->numSubTree = a->numSubTree;
		
		newNode->leftChild = copyTree(a->leftChild);
		newNode->rightChild = copyTree(a->rightChild);
	}
	else return NULL;
	return newNode;
}


node* createTree(int depth)
{
	if (depth == 0)
	{
		//impossible to be an operator, so the type is only 0 or 1
		node* newNode = new node;
		newNode->type = rand()%2;
		//if type = 0 so this is a variable, do nothing
		if (newNode->type == 1) //a constant
		{
			//assign the value
			newNode->value = fRand(-2.0,2.0);
		}
		return newNode;
	}
	
	node* newTree = new node;
	
	newTree->type = rand()%3;
	if (newTree->type == 0)
	{
		//variable, doesnt have any child, do nothing
	}
	else if (newTree->type == 1) //a constant, assign its value, no children
	{
		newTree->value = fRand(-2.0,2.0);
	}
	else
	{
		//operator, there must be two children
		newTree->value = rand()%4 ;

		node* newLeft = new node;
		node* newRight = new node;

		newLeft = createTree(depth-1);
		do
		{
			newRight = createTree(depth-1);
		} while ((fabs(newTree->value - 1) < EPS) && (fabs(newRight->value)<EPS) && (fabs(newRight->type-1)<EPS));

		newTree->leftChild = newLeft;
		newTree->rightChild = newRight;
		newTree->numSubTree += newLeft->numSubTree + newRight->numSubTree;
	}
	return newTree;
}

node* createTree2() //can be operator (3 nodes) or only 1 node (const or number)
{
	node* newNode = new node;
	newNode->type = rand()%3;
	//if type = 0 so this is a variable, do nothing
	if (newNode->type == 1) //a constant
	{
		//assign the value
		newNode->value = fRand(-2.0,2.0);
	}
	else if (newNode->type == 2) //an operator
	{
		//assign the operator
		newNode->value = (double) ( rand()%4 );
		node* newLeft = new node;
		node* newRight = new node;

		newLeft = createTree(0);
		do
		{
			newRight = createTree(0);
		} while ((fabs(newNode->value - 1) < EPS) && (fabs(newRight->value)<EPS) && (fabs(newRight->type-1)<EPS));

		newNode->leftChild = newLeft;
		newNode->rightChild = newRight;
		newNode->numSubTree += newLeft->numSubTree + newRight->numSubTree;
	}
	return newNode;
}

void printTree(node* tmp, int level)
{
	if (tmp != NULL)
	{
    	for (int i=0 ; i<level ; i++) cout << "    ";
		if (tmp->type == 0) cout << "x" << endl;
		else if (tmp->type == 1) cout << tmp->value << endl;
		else
		{
			if (fabs(tmp->value) < EPS) cout << "Kali" << endl;
			else if (fabs(tmp->value - 1.0) < EPS) cout << "Bagi" << endl;
			else if (fabs(tmp->value - 2.0) < EPS) cout << "Tambah" << endl;
			else cout << "Kurang" << endl;
		}
		//cout << tmp->type << " " << tmp->value << endl;
		printTree(tmp->leftChild, level+1);
		printTree(tmp->rightChild, level+1);
	}
}

bool isLeaf(node* a)
{
	return a->leftChild == NULL;
}

bool isEqualNode(node* a, node* b) //assume a and b are not NULL
{
	if (a->type == b->type && fabs(a->value - b->value) < EPS) return true;
	else return false;
}

int fix(node* &a)
{
	if (isLeaf(a))
	{
		a->numSubTree = 1;
		return 1;
	}
	else
	{
		return a->numSubTree = 1 + fix(a->leftChild) + fix(a->rightChild);
	}
}

double func(node* tmp, double x)
{
	//semua yg type = 0 valuenya jadi x
	if (tmp->leftChild == NULL)
	{
		if (tmp->type == 0) return x;
		else if (tmp->type == 1) return tmp->value;
	}
	else
	{
		double left = func(tmp->leftChild, x);
		double right = func(tmp->rightChild, x);

		double result = 0;
		if (fabs(tmp->value) < EPS) result = left*right;
		else if (fabs(tmp->value - 2) < EPS) result = left+right;
		else if (fabs(tmp->value - 3) < EPS) result = left-right;
		else
		{
			if (fabs(right) < EPS) result = -100000000;
			else result = left/right;
		}
		return result;
	}
}

double fitness(node* tmp)
{
	double mse = 0.0;
	for (int i=0 ; i<numInput ; i++)
	{
		double cur = func(tmp, xValues[i]);
		mse += ((yValues[i] - cur)*(yValues[i] - cur));
	}
	return mse;
}

bool cmpTree(node* a, node* b)
{
	return fitness(a) < fitness(b);
}

bool cmp(double a, double b)
{
	return a<b;
}

void crossover(node* a, node* b, node* &aa, node* &bb)
{
	aa = copyTree(a);
	bb = copyTree(b);

	//choose 1 node in A (can be root) to be moved to B
	int tmp1 = (rand()%(aa->numSubTree))+1;
	bool a_leaf = false;
	bool a_root = false;
	node* cur1 = aa;
	node* child1;
	bool dir1 = 0;
	//cout<<"Nilai tmp1 : " << tmp1 << endl;
	if (isLeaf(aa))
	{
		a_leaf = true;
		a_root = true;
		cur1 = aa;
	}
	else if (tmp1 == cur1->leftChild->numSubTree+1)
	{
		//A's root is moved, do nothing yet
		a_root = true;
	}
	else
	{
		child1 = cur1;
		while (!isLeaf(child1) && tmp1 != 1+child1->leftChild->numSubTree)
		{
			if (tmp1 <= child1->leftChild->numSubTree)
			{
				dir1 = 0;
				cur1 = child1;
				child1 = child1->leftChild;
			}
			else
			{
				dir1 = 1;
				tmp1 = tmp1 - 1 - child1->leftChild->numSubTree;
				cur1 = child1;
				child1 = child1->rightChild;
			}
		}
		//cout << cur1->type << "," << cur1->value<<endl;
		//cout << child1->type << "," << child1->value<<endl;
	}

	//choose 1 node in B (can be root) to be moved to A
	int tmp2 = (rand()%(bb->numSubTree))+1;
	bool b_leaf = false;
	bool b_root = false;
	node* cur2 = bb;
	node* child2;
	bool dir2 = 0;
	//cout<<"Nilai tmp2 : " << tmp2 << endl;
	if (isLeaf(bb))
	{
		b_leaf = true;
		b_root = true;
		cur2 = bb;
	}
	else if (tmp2 == cur2->leftChild->numSubTree+1)
	{
		//B's root is moved, do nothing yet
		b_root = true;
	}
	else
	{
		child2 = cur2;
		while (!isLeaf(child2) && tmp2 != 1+child2->leftChild->numSubTree)
		{
			if (tmp2 <= child2->leftChild->numSubTree)
			{
				dir2 = 0;
				cur2 = child2;
				child2 = child2->leftChild;
			}
			else
			{
				dir2 = 1;
				tmp2 = tmp2 - 1 - child2->leftChild->numSubTree;
				cur2 = child2;
				child2 = child2->rightChild;
			}
		}
		//cout << cur2->type << "," << cur2->value<<endl;
		//cout << child2->type << "," << child2->value<<endl;
	}

	//interchange cur1 and cur2
	node* temp;
	if (a_root && b_root)
	{
		temp = aa;
		aa = bb;
		bb = temp;
	}
	else if (a_root && !b_root)
	{
		temp = aa;
		aa = child2;
		if (dir2 == false) cur2->leftChild = temp;
		else cur2->rightChild = temp;
	}
	else if (!a_root && b_root)
	{
		temp = bb;
		bb = child1;
		if (dir1 == false) cur1->leftChild = temp;
		else cur1->rightChild = temp;
	}
	else
	{
		if (dir1 == false && dir2 == false)
		{
			temp = child1;
			cur1->leftChild = child2;
			cur2->leftChild = temp;
		}
		else if (dir1 == false && dir2 == true)
		{
			temp = child1;
			cur1->leftChild = child2;
			cur2->rightChild = temp;	
		}
		else if (dir1 == true && dir2 == false)
		{
			temp = child1;
			cur1->rightChild = child2;
			cur2->leftChild = temp;
		}
		else
		{
			temp = child1;
			cur1->rightChild = child2;
			cur2->rightChild = temp;
		}
	}

	//fix numSubTree of the offsprings
	fix(aa);
	fix(bb);

	//printTree(aa,0);
	//cout << "jumlah node : "<<aa->numSubTree << endl;
	//printTree(bb,0);
	//cout << "jumlah node : "<<bb->numSubTree << endl;
}

void mutation(node* &a)
{
	if (isLeaf(a))
	{
		node* newNode = new node;
		do
		{
			newNode = createTree2();
		} while (isEqualNode(a, newNode));
		a = newNode;
	}
	else
	{
		//randomly choose a leaf
		node* cur = a;
		int tmp = rand()%2;
		node* child;
		if (tmp==0) child = cur->leftChild;
		else child = cur->rightChild;
		//cout << cur->type<<","<<cur->value << " " << child->type<<","<<child->value << endl;
		while (!isLeaf(child)) //while the child is not a leaf, continue going down
		{
			tmp = rand()%2;
			if (tmp==0)
			{
				cur = child;
				child = child->leftChild;
			}
			else
			{
				cur = child;
				child = child->rightChild;
			}
			//cout << cur->type<<","<<cur->value << " " << child->type<<","<<child->value << endl;
		}

		node* newNode = new node;
		if (tmp==0)
		{
			do
			{
				newNode = createTree2();
			} while (isEqualNode(child, newNode));
			cur->leftChild = newNode;
		}
		else
		{
			do
			{
				newNode = createTree2();
			} while (isEqualNode(child, newNode));
			cur->rightChild = newNode;
		}
	}
}

int countLevel(node* a)
{
	if (a == NULL) return 0;
	else return 1+max( countLevel(a->leftChild), countLevel(a->rightChild) );
}

void generatePopulation()
{
	for (int i=0 ; i<numPopulation ; i++)
	{
		node* newTree = new node;
		do
		{
			newTree = createTree(5);
		} while (countLevel(newTree) <= 2);
		population.push_back(newTree);
		populationFitness.push_back(fitness(newTree));
	}

	printf("Populasi awalnya adalah :\n");
	for (int i=0 ; i<numPopulation ; i++)
	{
		printTree(population[i], 0);
		printf("+++++++++++++++++++++\n");
	}
}

int main()
{
	srand(time(NULL));
	cin >> numInput;

	for (int i=0 ; i<numInput ; i++)
	{
		double tx, ty;
		cin >> tx >> ty;
		xValues.push_back(tx);
		yValues.push_back(ty);
	}

	generatePopulation();

	printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	//proses utama
	for (int iter = 0 ; iter < numIteration ; iter++)
	{
		//kawin
		//nyari 4 pasang buat dikawinin berbeda, milih parentnya acak
		set< pair<int, int> > flag;		
		for (int i=0 ; i<4 ; i++)
		{
			int firstChromosome=0, secondChromosome=0;
			do
			{
				firstChromosome = iRand(0, numPopulation-1);
				do
				{
					secondChromosome = iRand(0, numPopulation-1);
				} while (secondChromosome == firstChromosome);
				if (secondChromosome < firstChromosome) swap(firstChromosome, secondChromosome);
			} while (flag.count( make_pair(firstChromosome, secondChromosome) ) > 0);
			flag.insert(make_pair(firstChromosome, secondChromosome));
			//cout << firstChromosome << " " << secondChromosome << endl;

			node* offspring1 = new node;
			node* offspring2 = new node;
			crossover(population[firstChromosome], population[secondChromosome], offspring1, offspring2);
			//printTree(offspring1, 0);
			//printTree(offspring2, 0);			
			int random = rand()%2;
			if (random == 0) mutation(offspring1);
			else mutation(offspring2);

			population.push_back(offspring1);
			population.push_back(offspring2);

			populationFitness.push_back(fitness(offspring1));
			populationFitness.push_back(fitness(offspring2));			
		}
		//seleksi alam
		sort(population.begin(), population.end(), cmpTree);
		sort(populationFitness.begin(), populationFitness.end(), cmp);
		int len = population.size();
		while (len>10)
		{
			population.pop_back();
			populationFitness.pop_back();
			len--;
		}
		printTree(population[0], 0);
		printf("Fitness terbaik pada perulangan ke-%d adalah : %lf\n",iter+1, populationFitness[0]);
		printf("----------------------\n");
	}

	return 0;
}