//Author: Reese Zimmermann
#include <iostream>
#include <vector>
#include <algorithm>
#include <exception>
using namespace std;

//Exception class 
class NotFoundException : public exception {
public:
    const char* what() const throw() {
        return "Value not found!";
    }
};
class duplicateInsertion : public exception {
public:
    const char* what() const throw() {
        return "Duplicate insertion!";
    }
};

//M-Tree class
template <class DT>
class MTree {
protected:
    int M; //Maximum number of children per node (M+1 way to split)
    vector<DT> values; // values stored in the node (M-1 values)
    vector<MTree*> children; //child pointers

public:
    MTree(int M); //constructor
    ~MTree(); //destructor
    bool is_leaf() const; //check if current node is a leaf
    void insert(const DT& value); //insert a value into the MtTree
    void split_node(); //split node if >=M
    MTree* find_child(const DT& value); //find the correct child node for a value
    bool search(const DT& value); //search for a value in the MTree
    void remove(const DT& value); //remove a value from the MTree
    //void rebuild(vector<DT>& input_values); //Build the Tree
    void rebuild(const vector <DT>& input_values); //Build the Tree
    vector<DT> collect_values() const; //collect values from all leaf nodes
    bool findValue(const DT& value); //find a value in the MTree
};

//constructor
template <class DT>
MTree<DT>::MTree(int M) : M(M){}

//destructor
template <class DT>
MTree<DT>::~MTree() {
    for(int i = 0; i < children.size(); i++){ //for each child node
        delete children[i];
    }
}
//check if current node is a leaf node
template <class DT>
bool MTree<DT>::is_leaf() const {
    return children.size() == 0; //if the node has no children, it is a leaf (childen.empty()?)
}
/** INSERTION
 * during insert, the new value is added to appropriate leaf node
 * if the leaf node esceeds M values, node performs a split operation
 */
template <class DT>
void MTree<DT>::insert(const DT& value){
    if(is_leaf()){
        if(findValue(value)){
            throw duplicateInsertion();
        }
        values.push_back(value); //add the value to the node
        sort(values.begin(), values.end()); //sort the values
        cout << "The value = " << value << " has been inserted." << endl; 
    } else{
        bool nodeFound = false;
        for(int i = 0; i < values.size(); i++){ //for each value in the node
            if(value <= values[i]){ //if the value is less than the current value
                children[i]->insert(value); //insert the value into the child node
                nodeFound = true;
                break;
            }
        }
        if(!nodeFound){ //if the value is greater than all the values in the node
            children[children.size()-1]->insert(value); //insert the value into the last child node
        }
    }
}
template <class DT>
void MTree<DT>::split_node(){ //algorithm to split the node if it exceeds M
    int mid = values.size()/2; //midpoint of the values
    DT midValue = values[mid]; //mid value

    MTree* leftChild = new MTree(M); //create a new left child node
    MTree* rightChild = new MTree(M); //create a new right child node

    leftChild->values.assign(values.begin(), values.begin() + mid); //assign the left values
    rightChild->values.assign(values.begin() + mid + 1, values.end()); //assign the right values

    if(!is_leaf()){
        leftChild->children.assign(children.begin(), children.begin() + mid + 1); //assign the left children
        rightChild->children.assign(children.begin() + mid + 1, children.end()); //assign the right children
    }
    values = {midValue}; //set the values to the mid value
    children = {leftChild, rightChild}; //set the children to the left and right children
}

//find_child: finds the correct child node for a value
template <class DT>
MTree<DT>* MTree<DT>::find_child(const DT& value){
    for(int i = 0; i < values.size(); i++){ //for each value in the node
        if(value < values[i]){ //if the value is less than the current value
            return children[i]; //return the child node
        }
    }
    return children[values.size()]; //return the last child node
}

/** SEARCH
 * search operation traverses the internal nodes using the split values 
 *      to determind the correct child pointer
 * once leaf node is reached, search operation checks for the value 
 *      within the sorted values of the leaf node.
 */
template<class DT>
bool MTree<DT>::search(const DT& value){
    if(is_leaf()){
        bool found = false;
        for(int i = 0; i < values.size(); i++){ //for each value in the node
            if(value == values[i]){ //if the value is found
                found = true;
                break;
            }
        }
        return found;
    } 
    bool found = false;
    for(int i = 0; i < values.size(); i++){ //for each value in the node
        if(value == values[i]){ //if the value is less than the current value
            return true; //search the child node
        }
        if(value < values[i]){ //if the value is less than the current value
            found = children[i]->search(value);
            if(found){
                break;
            }
        }
    }
    if(!found){
        found = children[children.size()-1]->search(value);
    }
    return found;
    
}

/** REMOVE (DELETION)
 *  deleting a value from the leaf node involves removing the value from the sorted array 
 *      in the leaf.
 */
template<class DT>
void MTree<DT>::remove(const DT& value){
    if(is_leaf()){
        bool found = false;
        for(int i = 0; i < values.size(); i++){ //for each value in the node
            if(value == values[i]){ //if value is found
                values.erase(values.begin() + i); //erase the value
                found = true;
                break;
            }
        }
        if(!found){ //added to fix the issue of not finding the value when called again in the input
            throw NotFoundException();
        }
        return;
    } else {
        bool found = false;
        for(int i = 0; i < values.size(); i++){ //for each value in the node
            if(value <= values[i]){ //if the value is less than the current value
                children[i]->remove(value); //remove the value from the child node
                found = true;
                break;
            }
        }
        if(!found){
            children[children.size()- 1]->remove(value);
        }
    }
}
// collect_values: collects all the values in the leaf nodes
template<class DT>
vector<DT> MTree<DT>::collect_values()const {
    vector<DT> result; //result vector
    if(is_leaf()){
        return values; //return the values in the node
    } else{
        for(int i = 0; i < children.size(); i++){ //for each child node
            vector<DT> childValues = children[i]->collect_values(); //collect the values in the child node
            result.insert(result.end(), childValues.begin(), childValues.end()); //add the values to the result vector
        }
    }
    return result;
}


/** REBUILD - DELETED CLASS (NOT USED) STATED AS BUILD TREE CLASS
 *  the operation collects all the values in the leaf nodes, merges them into a single sorted array
 * and reconstructs the tree from scratch
 */

// template<class DT>
// void MTree<DT>::rebuild(vector<DT>& input_values){
//     values.clear(); //clear the values in the node
//     children.clear(); //clear the children
//     rebuild(input_values); //build the tree from the input values
// }

/*rebuild: partitions the input values into M parts, assigns the end values of each partition 
to the current node and recursively builds child nodes for each partition
*/
template <class DT>
void MTree<DT>::rebuild(const vector <DT>& input_values){
    if(input_values.size() <= M-1){ //if size of input values is less than M-1
        values = input_values; //store the values in the node
    } else{
        int D = input_values.size()/M; //D is the number of values in each child node
        for(int i = 0; i < M; i++){ //for each child node
            int start = D * i; //start index of the child node
        
            int end; //end index of the child node
            if(i == M-1){ //if it is the last child node
                end = input_values.size() - 1; //end index is the last index 
            } else {
                end = start + D - 1;
                values.push_back(input_values[end]); //add the end value of the partition to the values of the node
            }

            //create a sub-vector for the current partition
            vector<DT> child_values(input_values.begin() + start, input_values.begin() + end + 1);
            MTree<DT>* child = new MTree<DT>(M); //create a new child node
            child->rebuild(child_values); //recursively build the tree for the child node
            children.push_back(child); //add the child node to the children vector
        }

    }
}

//find: finds a value in the MTree
template<class DT>
bool MTree<DT>::findValue(const DT& value){
    return search(value);
}

/****** main method ********/
int main(){
    int n; // # of numbers in the initial sorted array
    int MValue; // maximum number of children per node
    int numCommands; // number of commands to process
    char command; // command type
    int value; // value to insert, remove, or find

    cin >> n; // read the size of the sorted array
    vector<int> mySortedValues(n); //vector of size n

    //read n numbers from the input and add them to the vector mySortedValues (NOT COMPLETE)
    for(int i = 0; i < n; i++) {
        cin >> mySortedValues[i];
    }

    //Get the m values
    cin >> MValue;
    MTree<int>* myTree = new MTree<int>(MValue); //Create the M-Tree

    //build the tree
    myTree->rebuild(mySortedValues);

    // read the number of commands
    cin >> numCommands;

    /*********************************read each command process *************************************/
    //process each command: insert,delete,find...
    for(int i = 0; i < numCommands; i++) {
        cin >> command; //read command type
        switch(command){
            case 'I': {//insert
                cin >> value; //read value to insert
                try{
                    myTree->insert(value); //insert the value
                } catch(duplicateInsertion& e){
                    cout << "The value = " << value << " already in the tree." << endl;
                }
                break;
            }
            case 'R':{ //remove
                cin >> value;;
                try{
                    myTree->remove(value);
                    cout << "The value = " << value << " has been removed." << endl;
                } catch(NotFoundException& e){
                    cout << "The value = " << value << " not found." << endl;
                }
                break;
            }
            case 'F': { //find
                cin >> value;
                if(myTree->findValue(value)){
                    cout << "The element with value = " << value << " was found." << endl;
                } else{
                    cout << "The element with value = " << value << " not found." << endl;
                }
                break;
            }
            case 'B' : { //rebuild
                vector<int> myValues = myTree->collect_values();
                delete myTree;
                myTree = new MTree<int>(MValue);
                myTree->rebuild(myValues);
                cout << "The tree has been rebuilt." << endl;
                myValues.clear();
                break;
            }
            default:
                cout << "Invalid command." << endl;
        }
    }
    vector<int> finalValues = myTree->collect_values();
    cout << "Final list: ";
    for(int i = 0; i < finalValues.size(); i++){
        cout << finalValues[i] << " ";
        if((i+1) % 20 == 0){
            cout << endl;
        }
    }
    delete myTree;
    return 0;
}