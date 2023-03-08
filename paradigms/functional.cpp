#include <iostream>
#include <vector>

using namespace std;

// todo: struct Node
struct Node {
public:
  int value;
  Node* left;
  Node* right;

  Node(int value, Node* left = nullptr, Node* right = nullptr) {
    this->value = value;
    this->left = left;
    this->right = right;
  }
};

// todo: main algorithm
int getMiddle(int start, int end) {
  return (start + end) / 2;
}

Node* createNewNodeRef(vector<int> numbers, Node** refs, int start, int end) {
  return new Node(numbers[getMiddle(start, end)]);
}

Node* getNodeRef(Node** refs, int start, int end) {
  return refs[getMiddle(start, end)];
}

Node* createNodeChild(Node* getBalancedTreeRoot(vector<int> numbers, Node** refs, int start, int end),
                      vector<int> numbers, Node** refs, Node* node, int start, int end) {
  return getBalancedTreeRoot(numbers, refs, start, end);
}

Node* createNode(Node* getBalancedTreeRoot(vector<int> numbers, Node** refs, int start, int end), vector<int> numbers,
                 Node** refs, int start, int end) {
  return new Node(
      numbers[getMiddle(start, end)],
      createNodeChild(getBalancedTreeRoot, numbers, refs, refs[getMiddle(start, end)], start,
                      getMiddle(start, end) - 1),
      createNodeChild(getBalancedTreeRoot, numbers, refs, refs[getMiddle(start, end)], getMiddle(start, end) + 1, end));
}

Node* getBalancedTreeRoot(vector<int> numbers, Node** refs, int start, int end) {
  refs = refs == nullptr ? new Node*[numbers.size()] : refs;

  return start > end ? nullptr : createNode(getBalancedTreeRoot, numbers, refs, start, end);
}

Node* createTree(vector<int> numbers) {
  return getBalancedTreeRoot(numbers, nullptr, 0, numbers.size() - 1);
}

// todo: pass tree
template <typename T> void print(T value) {
  cout << value << " ";
}

template <typename T> void printAndReturn(T value) {
  print(value);
  return;
}

bool checkChildsExistence(Node* node) {
  return node->left != nullptr || node->right != nullptr;
}

bool isNodeExist(Node* node) {
  return node != nullptr;
}

void passNodeChildren(void passTree(Node* root), Node* root) {
  print(root->value);
  passTree(root->left);
  passTree(root->right);
}

void passTree(Node* root) {
  if (!isNodeExist(root))
    return printAndReturn("null");

  if (!checkChildsExistence(root))
    return printAndReturn(root->value);

  passNodeChildren(passTree, root);
}

// todo: input numbers
vector<int> pushToVector(vector<int> v, int a) {
  v.push_back(a);
  return v;
}

template <typename T> bool compareVariableWithValue(T var, T value) {
  return var == value;
}

template <typename T> T readValue() {
  T a;
  cin >> a;
  return a;
}

vector<int> recursionInput(vector<int> numbers) {
  int number = readValue<int>();

  return !compareVariableWithValue(readValue<char>(), ',') ? pushToVector(numbers, number)
                                                           : recursionInput(pushToVector(numbers, number));
}

vector<int> createNumberList() {
  vector<int> numbers;
  return recursionInput(numbers);
}

int main() {
  freopen("input.txt", "r", stdin);
  passTree(createTree(createNumberList()));

  return 0;
}
