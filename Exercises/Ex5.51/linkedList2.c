#include<stdio.h>
#include<stdlib.h>
#define line printf("====================================================================\n");

/* Let's make it more generic without requiring globals */

  
typedef struct linkedList_node {
  int data;
  struct linkedList_node *next;
} linkedList_node;

typedef struct linkedList {
  linkedList_node *head;
  struct linkedList *self;
} linkedList;

int linkedList_insert_begin(linkedList* l, int val) {
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

int linkedList_insert_middle_num(linkedList *l, int val,int pos) {
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

int linkedList_insert_middle_info(linkedList *l, int val,int loc) {
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
      if(temp->data==loc) {
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

int linkedList_insert_end(linkedList *l, int val) {
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
    free(temp);
    if(temp==l->head) {
      l->head=temp->next;
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
	  free(temp);
	  flag=1;
	  return 0;
	} else {
	  var1=temp->next;
	  temp=var->next;
	  var->next=var1;
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

int linkedList_del_middle_info(linkedList *l, int loc) {
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
	  free(temp);
	  flag=1;
	  return 0;
	} else {
	  var1=temp->next;
	  temp=var->next;
	  var->next=var1;
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
      printf("%d->",trav->data);
      trav=trav->next;
    }
    printf("\n");
    line;
  }
}

int linkedList_update(linkedList *l, int loc, int val) {
  int flag=0;
  linkedList_node *temp,*var1;
  temp=l->head;
  while(temp!=NULL) {
    if(temp->data==loc) {
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

int linkedList_traverse(linkedList *l, int val) {
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
	printf("The %d Element is %d Position\n",val,count);
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
      linkedList_insert_begin(l.self,val);
      linkedList_display(l.self);
      break;
    case 2:
      printf("Enter a Location Value After You Want To Insert Val: ");
      scanf("%d",&loc);
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      linkedList_insert_middle_info(l.self,val,loc);
      linkedList_display(l.self);
      break;
    case 3:
      printf("Enter a Position After You Want To Insert Val: ");
      scanf("%d",&pos);
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      linkedList_insert_middle_num(l.self,val,pos);
      linkedList_display(l.self);
      break;
    case 4:
      printf("Enter Value to Insert a linkedList_node: ");
      scanf("%d",&val);
      linkedList_insert_end(l.self,val);
      linkedList_display(l.self);
      break;
    case 5:
      linkedList_del_begin(l.self);
      linkedList_display(l.self);
      break;
    case 6:
      printf("Enter a Location Value You Want To Delete Val: ");
      scanf("%d",&loc);
      linkedList_del_middle_info(l.self,loc);
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
      linkedList_update(l.self,loc,val);
      linkedList_display(l.self);
      break;
    case 10:
      printf("Enter The Value you Want To Search: ");
      scanf("%d",&val);
      linkedList_traverse(l.self,val);
      linkedList_display(l.self);
      break;
    case 11:
      linkedList_display(l.self);
      break;
    }
  } while(ch!=12);
}
