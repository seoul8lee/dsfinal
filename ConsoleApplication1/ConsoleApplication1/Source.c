#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>


typedef struct NodeType {
	bool color; //red=true, blk=false
	int key;
	struct NodeType* left;
	struct NodeType* right;
	struct NodeType* p; //parent
}Node;

typedef struct NodeType* NodePtr;

typedef struct TreeType {
	struct NodeType* nil;
	struct NodeType* root;
}Tree;

typedef struct TreeType* TreePtr;

typedef struct StkType {
	int data;
	struct StkType* prev;
}Stk;

typedef struct StkType* StkPtr;

typedef struct HeadType {
	int cnt;
	struct StkType* top;
}Head;

typedef struct HeadType* HeadPtr;

HeadPtr head_new() {
	HeadPtr h = (HeadPtr)malloc(sizeof(Head));
	h->cnt = 0;
	h->top = NULL;
}

void stk_push(HeadPtr h, int n) {
	StkPtr s = (StkPtr)malloc(sizeof(Stk));
	s->data = n;

	if (h->top != NULL)
		s->prev = h->top;

	h->cnt++;
	h->top = s;
}


// t->nil 생성
NodePtr rbt_nil() {
	NodePtr node = (NodePtr)malloc(sizeof(Node));
	node->color = false;
	node->key = 0;
	node->left = node;
	node->right = node;
	node->p = node;
	return node;
}

// t 생성
TreePtr rbt_init() {
	TreePtr tree = (TreePtr)malloc(sizeof(Tree));
	tree->nil = rbt_nil();
	tree->root = tree->nil;
	return tree;
}

// node 생성
NodePtr node_alloc(TreePtr t, int n) {
	NodePtr node = (NodePtr)malloc(sizeof(Node));
	node->color = true;
	node->key = n;
	node->left = t->nil;
	node->right = t->nil;
	node->p = t->nil;

	return node;
}


void left_rotate(TreePtr t, NodePtr x) {
	if (x->right == t->nil)
		return;

	//set y
	NodePtr y = x->right;

	x->right = x->right->left;

	//turn y's left subtree into x's right subtree
	if (y->left != t->nil)
		y->left->p = x;

	y->p = x->p;

	//link x's parent to y
	if (x->p == t->nil)
		t->root = y;
	else {
		if (x == x->p->left)
			x->p->left = y;
		else
			x->p->right = y;
	}

	//put x on y's left
	y->left = x;
	x->p = y;

}

void right_rotate(TreePtr t, NodePtr y) {
	if (y->left == t->nil)
		return;

	//set x
	NodePtr x = y->left;


	//turn x'right subtree into y's left subtree
	if (x->right != t->nil)
		x->right->p = y;

	y->left = x->right;
	x->p = y->p;

	//link y's parent to x
	if (y->p == t->nil)
		t->root = x;
	else {
		if (y == y->p->left)
			y->p->left = x;
		else
			y->p->right = x;
	}

	//put y on x's right
	x->right = y;
	y->p = x;

}



void rbt_insert_fix_up(TreePtr t, NodePtr z) {
	NodePtr uncle_pt = t->nil;

	while (z->p->color == true && z != t->root) {
		/*  Case : A
		Parent of z is left child of Grand-parent of z */
		if (z->p == z->p->p->left) {

			uncle_pt = z->p->p->right;

			/* Case : 1
			The uncle of z is also red
			Only Recoloring required */
			if (uncle_pt->color == true) {
				z->p->color = false;
				uncle_pt->color = false;
				z->p->p->color = true;
				z = z->p->p; //나중에 검사
			}
			else if (uncle_pt->color == false) {

				/* Case : 2
				z is right child of its parent
				Left-rotation required */
				if (z == z->p->right) {
					z = z->p;
					left_rotate(t, z);
				}

				/* Case : 3
				z is left child of its parent
				Right-rotation required */
				z->p->color = false;
				z->p->p->color = true;
				right_rotate(t, z->p->p);
			}
		}


		/*  Case : B
		Parent of z is right child of Grand-parent of z */
		else
		{

			uncle_pt = z->p->p->left;

			/* Case : 1
			The uncle of z is also red
			Only Recoloring required */
			if (uncle_pt->color == true) {
				z->p->color = false;
				uncle_pt->color = false;
				z->p->p->color = true;
				z = z->p->p; //나중에 검사
			}


			else if (uncle_pt->color == false) {
				/* Case : 2
				z is left child of its parent
				right-rotation required */
				if (z == z->p->left) {
					right_rotate(t, z->p);
					z = z->right;
				}

				/* Case : 3
				z is right child of its parent
				left-rotation required */
				z->p->color = false;
				z->p->p->color = true;
				left_rotate(t, z->p->p);
			}
		}

	}

	t->root->color = false;
}



void rbt_insert(TreePtr t, NodePtr z) {
	NodePtr y = t->nil;
	NodePtr x = t->root;

	// node z의 p 연결
	while (x != t->nil) {
		y = x;
		if (z->key < x->key)
			x = x->left;
		else
			x = x->right;
	}
	z->p = y;


	// p의 node z 연결
	if (y == t->nil)
		t->root = z;
	else if (z->key < y->key)
		y->left = z;
	else
		y->right = z;


	// node z 기본 설정
	z->left = t->nil;
	z->right = t->nil;
	z->color = true;

	rbt_insert_fix_up(t, z);
}



//삭제 연산
NodePtr rbt_find(TreePtr t, NodePtr node, int num) {
	while (node->key != num && node != t->nil) {

		if (node->key < num)
			node = node->right;
		else if (node->key > num)
			node = node->left;

	}
	if (node->key == num)
		return node;

	return t->nil;
}

NodePtr rbt_min(TreePtr t, NodePtr x) {
	if (x == t->nil)
		return x;

	while (x->left != t->nil)
		x = x->left;

	return x;
}

void rbt_transplant(TreePtr t, NodePtr u, NodePtr v) {
	if (u->p == t->nil)
		t->root = v;
	else if (u == u->p->left)
		u->p->left = v;
	else
		u->p->right = v;
	v->p = u->p;
}

void rbt_delete_fixup(TreePtr t, NodePtr x) {
	NodePtr w = NULL;
	while (x != t->root && x->color == false) {
		if (x == x->p->left) {
			w = x->p->right; //sibling

							 //case 1 : x's sibling w is red
			if (w->color == true) {
				w->color = false;
				x->p->color = true;
				left_rotate(t, x->p);
				w = x->p->right;
			}

			// x's sibling w is blk

			//case 2 : both w's children are blk
			if (w->left->color == false && w->right->color == false) {
				w->color = true;
				x = x->p;
			}

			//case 3 : w's left is red, w's right is blk
			else {

				if (w->right->color == false) {
					w->left->color = false;
					w->color = true;
					right_rotate(t, w);
					w = x->p->right;
				}
				w->color = x->p->color;
				x->p->color = false;
				w->right->color = false;
				left_rotate(t, x->p);
				x = t->root;

				//case 4 : w's right is red

			}
		}

		else {
			w = x->p->left;

			//case 1 : x's sibling w is red
			if (w->color == true) {
				w->color = false;
				x->p->color = true;
				right_rotate(t, x->p);
				w = x->p->left;
			}

			// x's sibling w is blk

			//case 2 : both w's children are blk
			if (w->right->color == false && w->left->color == false) {
				w->color = true;
				x = x->p;
			}

			//case 3 : w's right is red, w's left is blk
			else {

				if (w->left->color == false) {
					w->right->color = false;
					w->color = true;
					left_rotate(t, w);
					w = x->p->left;
				}


				//case 4 : w's left is red
				w->color = x->p->color;
				x->p->color = false;
				w->left->color = false;
				right_rotate(t, x->p);
				x = t->root;

			}
		}
	}
	x->color = false;
}


void rbt_delete(TreePtr t, NodePtr z) {
	if (z == t->nil) {
		return;
	}

	NodePtr y = z;
	NodePtr x = NULL;

	bool y_origin_color = y->color;


	if (z->left == t->nil) {
		x = z->right;
		rbt_transplant(t, z, z->right);
	}
	else if (z->right == t->nil) {
		x = z->left;
		rbt_transplant(t, z, z->left);
	}
	else {
		y = rbt_min(t, z->right);
		y_origin_color = y->color;
		x = y->right;

		if (y->p == z)
			x->p = y;
		else {
			rbt_transplant(t, y, y->right);
			y->right = z->right;
			y->right->p = y;
		}

		rbt_transplant(t, z, y);
		y->left = z->left;
		y->left->p = y;
		y->color = z->color;
	}

	if (y_origin_color == false)
		rbt_delete_fixup(t, x);

}




NodePtr bintree2listUtil(TreePtr t, NodePtr root)
{
	if (root == t->nil)
		return root;

	// Convert the left subtree and link to root
	if (root->left != t->nil)
	{
		// Convert the left subtree
		NodePtr left = bintree2listUtil(t, root->left);

		// Find inorder predecessor. After this loop, left
		// will point to the inorder predecessor
		for (; left->right != t->nil; left = left->right);

		// Make root as next of the predecessor
		left->right = root;

		// Make predecssor as previous of root
		root->left = left;
	}

	// Convert the right subtree and link to root
	if (root->right != t->nil)
	{
		// Convert the right subtree
		NodePtr right = bintree2listUtil(t, root->right);

		// Find inorder successor. After this loop, right
		// will point to the inorder successor
		for (; right->left != t->nil; right = right->left);

		// Make root as previous of successor
		right->left = root;

		// Make successor as next of root
		root->right = right;
	}

	return root;
}


NodePtr bintree2list(TreePtr t, NodePtr root)
{
	// Base case
	if (root == t->nil)
		return t->nil;

	// Convert to DLL using bintree2listUtil()
	root = bintree2listUtil(t, t->root);

	while (root->left != t->nil)
		root = root->left;

	return (root);
}




int rbt_fin(TreePtr t, NodePtr h, int num) {
	NodePtr tvl = h;
	if (tvl == t->nil)
		return 0;

	while (tvl->key < num && tvl != t->nil)
		tvl = tvl->right;

	if (tvl->key == num)
		return tvl->key;
	else if (tvl->key > num)
		return 0;

	return 0;
}



int rbt_pre(TreePtr t, NodePtr h, int num) {
	NodePtr tvl = h;
	NodePtr pretvl = h;

	if (tvl == t->nil)
		return 0;

	while (tvl->key < num && tvl != t->nil) {
		pretvl = tvl;
		tvl = tvl->right;
	}

	if (tvl->left == t->nil)
		return pretvl->key;
	
	return tvl->left->key;
}


int rbt_suc(TreePtr t, NodePtr h, int num) {
	NodePtr tvl = h;

	while (tvl->key <= num && tvl != t->nil)
		tvl = tvl->right;

	return tvl->key;

	if (tvl == t->nil)
		return 0;

}



int main() {
	TreePtr t = rbt_init();
	int data;
	int i = 0;
	int j = 0;
	int k = 0;

	FILE * fp1 = fopen("test01.txt", "r");
	FILE * fp2 = fopen("search01.txt", "r");
	FILE * fp3 = fopen("output01.txt", "w");


	if (fp1 == NULL || fp2 == NULL || fp3 == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}

	//insert
	while (fscanf_s(fp1, "%d", &data) != EOF) {
		if (data > 0)
			rbt_insert(t, node_alloc(t, data));
		else if (data < 0) {
			data = -1 * data;
			rbt_delete(t, rbt_find(t, t->root, data));
		}
		else if (data == 0)
			break;
	}
	fclose(fp1);


	NodePtr head = bintree2list(t, t->root);


	while (fscanf_s(fp2, "%d", &data) != EOF) {
		if (data > 0) {
			i = rbt_pre(t, head, data);
			j = rbt_fin(t, head, data);
			k = rbt_suc(t, head, data);

			if (i == 0 && j == 0 && k == 0)
				fprintf(fp3, "nil nil nil\n");
			else if (i == 0 && j == 0 && k != 0)
				fprintf(fp3, "nil nil %d\n", k);
			else if (i == 0 && j != 0 && k == 0)
				fprintf(fp3, "nil %d nil\n", j);
			else if (i == 0 && j != 0 && k != 0)
				fprintf(fp3, "nil %d %d\n", j, k);
			else if (i != 0 && j == 0 && k == 0)
				fprintf(fp3, "%d nil nil\n", i);
			else if (i != 0 && j == 0 && k != 0)
				fprintf(fp3, "%d nil %d\n", i, k);
			else if (i != 0 && j != 0 && k == 0)
				fprintf(fp3, "%d %d nil\n", i, j);
			else
				fprintf(fp3, "%d %d %d\n", i, j, k);
		}

		else if (data == 0)
			break;
	}



	fclose(fp2);
	fclose(fp3);

	return;
}