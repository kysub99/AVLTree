#include <iostream>
#include <algorithm>
#include <stack>
using namespace std;
#define MAX 100'001

struct Node {
	int height;
	int left;
	int right;
	int parent;
	int size;

	Node() {
		reset();
	}

	void reset() {
		height = 1;
		left = right = parent = 0;
		size = 1;
	}
};

Node tree[MAX];

class AVLTree {
public:
	int root;

	AVLTree() {
		root = 0;
	}

	void resetTree() {
		root = 0;
		for (int i = 0; i < MAX; ++i) {
			tree[i].reset();
		}
		tree[0].height = 0;
		tree[0].size = 0;
	}

	inline void updateHeight(int idx) {
		if (idx != 0) {
			int left = tree[idx].left;
			int right = tree[idx].right;
			tree[idx].height = max(tree[left].height, tree[right].height) + 1;
		}
	}

	inline void updateSize(int idx) {
		if (idx != 0) {
			tree[idx].size = 1;
			if (tree[idx].left != 0) {
				tree[idx].size += tree[tree[idx].left].size;
			}
			if (tree[idx].right != 0) {
				tree[idx].size += tree[tree[idx].right].size;
			}
		}
	}

	inline int getBalance(int idx) {
		int left = tree[idx].left;
		int right = tree[idx].right;		

		return tree[left].height - tree[right].height;
	}

	int leftRotate(int z) {
		int y = tree[z].right;
		tree[z].right = tree[y].left;
		if (tree[y].left != 0) {
			tree[tree[y].left].parent = z;
		}
		tree[y].left = z;

		int zParent = tree[z].parent;
		tree[z].parent = y;
		tree[y].parent = zParent;
		if (zParent == 0) {
			root = y;
		}
		else {
			if (tree[zParent].left == z)
				tree[zParent].left = y;
			else
				tree[zParent].right = y;
		}

		updateHeight(z);
		updateHeight(y);
		updateSize(z);
		updateSize(y);

		return y;
	}

	int rightRotate(int z) {
		int y = tree[z].left;
		tree[z].left = tree[y].right;
		if (tree[y].right != 0) {
			tree[tree[y].right].parent = z;
		}
		tree[y].right = z;

		int zParent = tree[z].parent;
		tree[z].parent = y;
		tree[y].parent = zParent;
		if (zParent == 0) {
			root = y;
		}
		else {
			if (tree[zParent].left == z)
				tree[zParent].left = y;
			else
				tree[zParent].right = y;
		}

		updateHeight(z);
		updateHeight(y);
		updateSize(z);
		updateSize(y);

		return y;
	}

	int restructure(int idx) {
		updateHeight(idx);
		updateSize(idx);
		int balance = getBalance(idx);

		if (balance > 1) {
			if (getBalance(tree[idx].left) < 0) {
				tree[idx].left = leftRotate(tree[idx].left);
			}
			return rightRotate(idx);
		}
		else if (balance < -1) {
			if (getBalance(tree[idx].right) > 0) {
				tree[idx].right = rightRotate(tree[idx].right);
			}
			return leftRotate(idx);
		}
		return idx;
	}

	void insertNode(int key) {
		if (root == 0) {
			root = key;
			return;
		}

		int cur = root;
		while (cur != 0) {
			if (key < cur) {
				if (tree[cur].left == 0) {
					tree[cur].left = key;
					tree[key].parent = cur;
					break;
				}
				cur = tree[cur].left;
			}
			else{
				if (tree[cur].right == 0) {
					tree[cur].right = key;
					tree[key].parent = cur;
					break;
				}
				cur = tree[cur].right;
			}			
		}

		cur = key;
		while (cur != 0) {
			cur = tree[cur].parent;
			cur = restructure(cur);
		}
	}

	inline void trans(int u, int v) {
		if (tree[u].parent == 0) {
			root = v;
		}
		else if (u == tree[tree[u].parent].left) {
			tree[tree[u].parent].left = v;
		}
		else {
			tree[tree[u].parent].right = v;
		}
		if (v != 0) {
			tree[v].parent = tree[u].parent;
		}
	}

	void deleteNode(int key) {
		if (root == 0 || root != key && tree[key].parent == 0 && tree[key].left == 0 && tree[key].right == 0) {
			return;
		}

		int cur = key;
		int par = tree[cur].parent;
		int rest = 0;
		bool twoChild = (tree[cur].left != 0 && tree[cur].right != 0);

		if (twoChild) {
			int successor = getSuccessor(tree[cur].right);
			if (tree[successor].parent == cur) {
				rest = successor;
			}
			else {
				rest = tree[successor].parent;
			}

			if (tree[successor].parent != cur) {
				trans(successor, tree[successor].right);
				tree[successor].right = tree[cur].right;
				tree[tree[cur].right].parent = successor;
			}

			trans(cur, successor);
			tree[successor].left = tree[cur].left;
			tree[tree[cur].left].parent = successor;
			updateHeight(successor);
			updateSize(successor);

			par = tree[successor].parent;

			if (root == cur) {
				root = successor;
			}
			cur = rest;
		}
		else {

			int child = tree[cur].left != 0 ? tree[cur].left : tree[cur].right;

			if (child != 0) {
				trans(cur, child);
				cur = tree[child].parent;
			}
			else {
				if (cur == root) {
					root = 0;
				}
				else {
					if (tree[par].left == cur)
						tree[par].left = 0;
					else if (tree[par].right == cur)
						tree[par].right = 0;
				}
				cur = tree[cur].parent;
			}
			cur = tree[key].parent;


		}

		tree[key].reset();

		while (cur != 0) {
			cur = restructure(cur);
			cur = tree[cur].parent;
		}
	}

	int getSuccessor(int idx) {
		while (tree[idx].left != 0) {
			idx = tree[idx].left;
		}
		return idx;
	}

	int findMinimum(int idx, int& depth) {
		int cur = idx;
		while (tree[cur].left != 0) {
			cur = tree[cur].left;
			depth++;
		}

		return cur;
	}

	int findMaximum(int idx, int& depth) {
		int cur = idx;
		while (tree[cur].right != 0) {
			cur = tree[cur].right;
			depth++;
		}

		return cur;
	}

	int getDepth(int idx) {
		if (idx != root && tree[idx].parent == 0)
			return -1;
		int depth = 0;
		while (idx != 0) {
			idx = tree[idx].parent;
			depth++;
		}

		return depth - 1;
	}

	int rank(int key) {
		int cur = root;
		int r = 0;

		while (cur != 0) {
			if (key >= cur) {
				if (tree[cur].left == 0)
					r += 1;
				else
					r += tree[tree[cur].left].size + 1;

				if (cur == key) {
					break;
				}
				cur = tree[cur].right;
			}
			else {
				cur = tree[cur].left;
			}
		}

		return r;
	}


};

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);

	int T;
	cin >> T;

	AVLTree avl;

	while (T--) {
		avl.resetTree();
		int Q;
		cin >> Q;

		for (int i = 0; i < Q; ++i) {
			string cmd;
			int x;
			cin >> cmd;

			if (cmd == "insert") {
				cin >> x;
				avl.insertNode(x);
				cout << avl.getDepth(x) << "\n";
			}
			else if (cmd == "erase") {
				cin >> x;
				int d = avl.getDepth(x);
				if (d == -1)
					cout << "0\n";
				else {
					cout << avl.getDepth(x) << "\n";
					avl.deleteNode(x);
				}

			}
			else if (cmd == "find") {
				cin >> x;
				int d = avl.getDepth(x);
				if (d == -1)
					cout << "0\n";
				else
					cout << d << "\n";
			}
			else if (cmd == "size") {
				cout << tree[avl.root].size << "\n";
			}
			else if (cmd == "empty") {
				cout << (avl.root == 0 ? 1 : 0) << "\n";
			}
			else if (cmd == "rank") {
				cin >> x;
				int d = avl.getDepth(x);
				if (d == -1)
					cout << "0\n";
				else
					cout << d << " " << avl.rank(x) << "\n";

			}
			else if (cmd == "minimum") {
				cin >> x;
				int depth = 0;
				int subDepth = avl.getDepth(x);
				int num = avl.findMinimum(x, depth);
				cout << num << " " << subDepth + depth << "\n";

			}
			else if (cmd == "maximum") {
				cin >> x;
				int depth = 0;
				int subDepth = avl.getDepth(x);
				int num = avl.findMaximum(x, depth);
				cout << num << " " << subDepth + depth << "\n";
			}
		}
	}

	return 0;
}
