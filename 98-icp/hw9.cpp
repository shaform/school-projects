#include <stdio.h>
#include <stdlib.h>

typedef struct node { int datum; struct node* succ; } node;    

void print(node* p)
{
    if (p==NULL) printf("\n");
    else { printf("%d ",p->datum); print(p->succ); }
}

bool find(node* p,int d)
{
    if (p==NULL) return false;
    else if (p->datum==d) return true;
    else return find(p->succ,d);
}

//void insert(node** p,int d)
//{
//  node* q=(node*)malloc(sizeof(node));
//   q->datum=d;
//         q->succ=*p;
//         *p=q;
//}

node* insert(node* p,int d)
{
    node* q=(node*)malloc(sizeof(node));
    q->datum=d;
    q->succ=p;
    return q;
}

//void erase(node** p,int d)
//{
//   node *q=*p,*r;
//   while (q!=NULL)
//         if (q->datum==d) break;
//         else { r=q; q=q->succ; }
//   if (q!=NULL) {
//         if (q==*p) *p=(*p)->succ;
//         else r->succ=q->succ;
//         free(q);
//   }
//}

node* erase(node* p,int d)
{
    if (p!=NULL)
        if (p->datum==d) { node* q=p; p=p->succ; free(q); }
        else p->succ=erase(p->succ,d);
    return p;
}

void eraseAll(node* p)
{
    if (p!=NULL) { eraseAll(p->succ); free(p); }
}

////////////////////////////////////////////////////////////

/* Additional Functions: */


size_t length(node* p)
{
    int n = 0;
    while (p!=NULL) {
        ++n;
        p = p->succ;
    }
    return n;
}

size_t lengthRec(node* p)
{
    if (p==NULL)
        return 0;
    else
        return lengthRec(p->succ) + 1;
}


void insertEnd(node** p,int d)
{
    node* q=(node*)malloc(sizeof(node));
    q->datum=d;
    q->succ=NULL;

    if (*p == NULL) {
        *p = q;
    } else {

        node *pte = *p;
        while (pte->succ != NULL)
            pte = pte->succ;

        pte->succ = q;
    }
}

node* insertEndRec(node* p,int d)
{
    if (p==NULL) {
        node* q=(node*)malloc(sizeof(node));
        q->datum=d;
        q->succ=NULL;
        return q;
    } else {
        p->succ = insertEndRec(p->succ, d);
        return p;
    }
}

void eraseLast(node** p,int d)
{
    node *q=*p, *r, *l=NULL, *rl;
    while (q!=NULL) {
        if (q->datum==d) {
            l = q;
            rl = r;
        }
        r=q;
        q=q->succ;
    }
    if (l!=NULL) {
        if (l==*p) *p=(*p)->succ;
        else rl->succ=l->succ;
        free(l);
    }
}
node* eLastRec(node* p, int d, bool* erasable)
{
    if  (p!=NULL) {
        p->succ = eLastRec(p->succ, d, erasable);

        if (p->datum==d && *erasable) {
            node* next = p->succ;
            free(p);

            *erasable = false;

            return next;
        }
    } else
        *erasable = true;

    return p;
}
node* eraseLastRec(node* p,int d)
{
    bool erasable = false;
    if (p!=NULL)
        p = eLastRec(p, d, &erasable);
    return p;
}


int main(void)            
{
    node* head=NULL;
    int ch; 
    printf("Command: ");
    while ((ch=getchar())!=EOF) {
        switch(ch) {
            int d;
            case 'd': 
            scanf("%d",&d); head=erase(head,d); break;
            case 'e': 
            scanf("%d",&d); eraseLast(&head,d); break;
            case 'E': 
            scanf("%d",&d); head=eraseLastRec(head,d); break;
            case 'i': 
            scanf("%d",&d); head=insert(head,d); break;
            case 'j':
            scanf("%d",&d); insertEnd(&head,d); break;
            case 'J':
            scanf("%d",&d); head=insertEndRec(head,d); break;
            case 'l':
            printf("length = %zu\n",length(head)); break;
            case 'L':
            printf("length = %zu\n",lengthRec(head)); break;
            case 'p': 
            print(head); break;
            case 's': 
            scanf("%d",&d);
            printf(find(head,d)? "Found\n": "Not found\n");
            break;
            default: continue;
        }
        printf("Command: ");
    }
    eraseAll(head); head=NULL;
    printf("List erased\n");
}
