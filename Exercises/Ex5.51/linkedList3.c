/* Allow for generic data type */

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#define line printf("====================================================================\n");

typedef void * node_data;

typedef enum { false, true } bool;

/*----------------------------------------------------------------------
 * A generic type
 *----------------------------------------------------------------------*/

typedef struct Int { int x; } Int;

node_data Int_new(int n, ...) {
  // we ignore the n
  va_list ap;
  va_start(ap,n);
  Int *d = (node_data) calloc(1,sizeof(Int));
  d->x = va_arg(ap,int);
  va_end(ap);
}

bool Int_equal(node_data n, node_data m) {
  if (((Int *) n)->x == ((Int *) m)->x)
    return true;
  else
    return false;
}

void Int_destroy(node_data i) { free((Int *) i); }

void Int_print(node_data n){
  printf("%d",((Int *) n)->x);
}

//----------------------------------------------------------------------

typedef struct linkedList_node {
  node_data data;
  struct linkedList_node *next;
} linkedList_node;

typedef struct linkedList {
  linkedList_node *head;
  node_data (*new) (int,...);
  bool (*equal)(node_data, node_data);
  void (*destroy)(node_data);
  void (*print)(node_data);
  struct linkedList *self;
} linkedList;

int linkedList_insert_begin(linkedList* l, node_data val) {
  linkedList_node *var =
    (linkedList_node*) malloc (sizeof(linkedList_node));
  var->data=val;
  if(l->head==NULL) {
    l->head=var;
    l->head->next=NULL;
  } else {
    var->next=l->head;
    l->head=var;
  }
}

int linkedList_insert_middle_num(linkedList *l, node_data val, int pos) {
  int count=0,flag=0;

  linkedList_node *var1, *temp, *var;
  var=(linkedList_node*)malloc(sizeof(linkedList_node));
  var->data=val;
  temp=l->head;

  if(l->head==NULL) {
    l->head=var;
    l->head->next=NULL;
  } else {
    while(temp!=NULL) {
      if(count==pos) {
	var1=temp->next;
	temp->next=var;
	var->next=var1;
	flag=1;
	return 0;
      }
      temp=temp->next;
      count++;
    }
    if(flag==0) {
      line;
      printf("\t\tThe LinkedList_Node Number Is Not Found...\n");
      line;
    }
  }
}

int linkedList_insert_middle_info(linkedList *l, node_data val, node_data loc) {
  int flag=0;
  linkedList_node *var1,*temp,*var;
  var=(linkedList_node*)malloc(sizeof(linkedList_node));
  var->data=val;
  temp=l->head;
  if(l->head==NULL) {
    l->head=var;
    l->head->next=NULL;
  } else {
    while(temp->next!=NULL) {
      if(l->equal(temp->data,loc)) {
	var1=temp->next;
	temp->next=var;
	var->next=var1;
	flag=1;
	return 0;
      }
      temp=temp->next;
    }
  }
  if(flag==0) {
    line;
    printf("\t\tThe Value Is Not Found...\n");
    line;
  }
}

int linkedList_insert_end(linkedList *l, node_data val) {
  linkedList_node *temp, *var;
  var=(linkedList_node*)malloc(sizeof(linkedList_node));
  var->data=val;
  temp=l->head;
  if(l->head==NULL) {
    l->head=var;
    l->head->next=NULL;
  } else {
    while(temp->next!=NULL) {
      temp=temp->next;
    }
    var->next=NULL;
    temp->next=var;
  }
}

int linkedList_del_begin(linkedList* l) {
  linkedList_node *temp;
  temp=l->head;
  if(l->head==NULL) {
    line;
    printf("\t\tLinked_List_is_an_Empty\n");
    line;
  } else {
    l->head=l->head->next;
    l->destroy(temp->data);
    free(temp);
  }
}

int linkedList_del_end(linkedList* l) {
  linkedList_node *temp, *var;
  temp=l->head;
  if(l->head==NULL) {
    line;
    printf("\t\tLinked_List_is_an_Empty\n");
    line;
  } else {
    while(temp->next!=NULL) {
      var=temp;
      temp=temp->next;
    }
    var->next=NULL;
    l->destroy(temp->data);
    free(temp);
    if(temp==l->head) {
      l->head=temp->next;
      l->destroy(temp->data);
      free(temp);
    }
  }
}

int linkedList_del_middle_num(linkedList *l, int pos) {
  int count=0,flag=0;
  linkedList_node *temp,*var1,*var;
  temp=l->head;
  if(l->head==NULL) {
    line;
    printf("\t\tLinked_is_an_Empty\n");
    line;
  } else {
    while(temp!=NULL) {
      if(count==pos) {
	if(temp==l->head) {
	  l->head=temp->next;
	  l->destroy(temp->data);
	  free(temp);
	  flag=1;
	  return 0;
	} else {
	  var1=temp->next;
	  temp=var->next;
	  var->next=var1;
	  l->destroy(temp->data);
	  free(temp);
	  return 0;
	}
      } else {
	var=temp;
	temp=temp->next;
	count++;
      }
    }
  }
  if(flag==0) {
    line;
    printf("\t\tThe LinkedList_Node Number Is Not Found...\n");
    line;
  }
}

int linkedList_del_middle_info(linkedList *l, node_data loc) {
  int flag=0;
  linkedList_node *temp,*var1,*var;
  temp=l->head;
  if(l->head==NULL) {
    line;
    printf("\t\tLinked_is_an_Empty\n");
    line;
  } else {
    while(temp!=NULL) {
      if(temp->data==loc) {
	if(temp==l->head) {
	  l->head=temp->next;
	  l->destroy(temp->data);
	  free(temp);
	  flag=1;
	  return 0;
	} else {
	  var1=temp->next;
	  temp=var->next;
	  var->next=var1;
	  l->destroy(temp->data);
	  free(temp);
	  return 0;
	}
      } else {
	var=temp;
	temp=temp->next;
      }
    }
  }
  if(flag==0) {
    line;
    printf("\t\tThe Value Is Not Found...\n");
    line;
  }
}

int linkedList_display(linkedList *l) {
  linkedList_node *trav=l->head;
  if(trav==NULL) {
    line;
    printf("\t\tLinked List is Empty\n");
    line;
  } else {
    line;
    while(trav!=NULL) {
      l->print(trav->data);
      printf("->");
      trav=trav->next;
    }
    printf("\n");
    line;
  }
}

int linkedList_update(linkedList *l, node_data loc, node_data val) {
  int flag=0;
  linkedList_node *temp,*var1;
  temp=l->head;
  while(temp!=NULL) {
    if(l->equal(temp->data,loc)) {
      temp->data=val;
      flag=1;
    }
    temp=temp->next;
  }
  if(flag==0) {
    line;
    printf("\t\tValue Not Found Please Try Again...\n");
    line;
  } else {
    line;
    printf("\t\tValue is Updated...\n");
    line;
  }
}

int linkedList_traverse(linkedList *l, node_data val) {
  int flag=0,count=1;
  linkedList_node *temp;
  temp=l->head;
  if(l->head==NULL) {
    line;
    printf("\t\tLinked_is_an_Empty\n");
    line;
  } else {
    while(temp!=NULL) {
      if(temp->data==val) {
	line;
	printf("The ");
	l->print(val);
	printf(" Element is %d Position\n",count);
	line;
	flag=1;
	return 0;
      }
      temp=temp->next;
      count++;
    }
  }
  if(flag==0) {
    line;
    printf("\t\tValue Not Found Please Try Again...\n");
    line;
  }
}

int main() {
  int ch,val,n,loc,pos;
  linkedList l;
  l.head = NULL;
  l.self = &l;
  l.equal = &Int_equal;
  l.new = &Int_new;
  l.destroy = &Int_destroy;
  l.print = &Int_print;
  
  node_data *locI;
  do {
    printf("\t\t\tSINGLY_LINKED_LIST\n");
    line;
    printf(" 1.Insert_at_Begining\n");
    printf(" 2.Insert_at_Middle_Based_on_Node_Information\n");
    printf(" 3.Insert_at_Middle_Based_on_Node_Number\n");
    printf(" 4.Insert_at_Ending\n");
    printf(" 5.Delete_at_Begining\n");
    printf(" 6.Delete_at_Middle_Based_on_Node_Information\n");
    printf(" 7.Delete_at_Middle_Based_on_Node_Number\n");
    printf(" 8.Delete_at_Ending\n");
    printf(" 9.Update_Node\n");
    printf("10.Travarse\n");
    printf("11.Display_Node\n");
    printf("12.Exit\n");
    line;
    printf("Enter Your Choice: ");
    scanf("%d",&ch);
    switch(ch) {
    case 1:
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      linkedList_insert_begin(l.self,l.new(1,val));
      linkedList_display(l.self);
      break;
    case 2:
      printf("Enter a Location Value After You Want To Insert Val: ");
      scanf("%d",&loc);
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      locI = l.new(1,loc);
      linkedList_insert_middle_info(l.self,l.new(1,val),locI);
      l.destroy(locI);
      linkedList_display(l.self);
      break;
    case 3:
      printf("Enter a Position After You Want To Insert Val: ");
      scanf("%d",&pos);
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      linkedList_insert_middle_num(l.self,l.new(1,val),pos);
      linkedList_display(l.self);
      break;
    case 4:
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      linkedList_insert_end(l.self,l.new(1,val));
      linkedList_display(l.self);
      break;
    case 5:
      linkedList_del_begin(l.self);
      linkedList_display(l.self);
      break;
    case 6:
      printf("Enter a Location Value You Want To Delete Val: ");
      scanf("%d",&loc);
      locI = l.new(1,loc);
      linkedList_del_middle_info(l.self,locI);
      l.destroy(locI);
      linkedList_display(l.self);
      break;
    case 7:
      printf("Enter a Position You Want To Delete Val: ");
      scanf("%d",&pos);
      linkedList_del_middle_num(l.self,pos);
      linkedList_display(l.self);
      break;
    case 8:
      linkedList_display(l.self);
      linkedList_del_end(l.self);
      linkedList_display(l.self);
      break;
    case 9:
      printf("Enter The Value You Want To Update: ");
      scanf("%d",&loc);
      printf("Enter The New Value To Update a Node Value: ");
      scanf("%d",&val);
      locI = l.new(1,loc);
      linkedList_update(l.self,locI,l.new(1,val));
      l.destroy(locI);
      linkedList_display(l.self);
      break;
    case 10:
      printf("Enter The Value you Want To Search: ");
      scanf("%d",&val);
      linkedList_traverse(l.self,l.new(1,val));
      linkedList_display(l.self);
      break;
    case 11:
      linkedList_display(l.self);
      break;
    }
  } while(ch!=12);
}
