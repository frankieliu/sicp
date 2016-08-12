#include<stdio.h>
#include<stdlib.h>
#define line printf("\n====================================================================\n");

struct node {
  int data;
  struct node *next;
} *var, *head, *trav;

int insert_begin(int val) {
  var = (struct node*) malloc (sizeof(struct node));
  var->data=val;
  if(head==NULL) {
    head=var;
    head->next=NULL;
  } else {
    var->next=head;
    head=var;
  }
}

int insert_middle_num(int val,int pos) {
  int count=1,flag=0;
  struct node *var1,*temp;
  var=(struct node*)malloc(sizeof(struct node));
  var->data=val;
  temp=head;
  if(head==NULL) {
    head=var;
    head->next=NULL;
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
      printf("\t\tThe Node Number Is Not Found...");
      line;
    }
  }
}

int insert_middle_inf(int val,int loc) {
  int flag=0;
  struct node *var1,*temp;
  var=(struct node*)malloc(sizeof(struct node));
  var->data=val;
  temp=head;
  if(head==NULL) {
    head=var;
    head->next=NULL;
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
    printf("\t\tThe Value Is Not Found...");
    line;
  }
}

int insert_end(int val) {
  struct node *temp;
  var=(struct node*)malloc(sizeof(struct node));
  var->data=val;
  temp=head;
  if(head==NULL) {
    head=var;
    head->next=NULL;
  } else {
    while(temp->next!=NULL) {
      temp=temp->next;
    }
    var->next=NULL;
    temp->next=var;
  }
}

int del_begin() {
  struct node *temp;
  temp=head;
  if(head==NULL) {
    line;
    printf("\nLinked_List_is_an_Empty");
    line;
  } else {
    head=head->next;
    free(temp);
  }
}

int del_end() {
  struct node *temp;
  temp=head;
  if(head==NULL) {
    line;
    printf("\t\tLinked_List_is_an_Empty");
    line;
  } else {
    while(temp->next!=NULL) {
      var=temp;
      temp=temp->next;
    }
    var->next=NULL;
    free(temp);
    if(temp==head) {
      head=temp->next;
      free(temp);
    }
  }
}

int del_middle_num(int pos) {
  int count=1,flag=0;
  struct node *temp,*var1;
  temp=head;
  if(head==NULL) {
    line;
    printf("\t\tLinked_is_an_Empty");
    line;
  } else {
    while(temp!=NULL) {
      if(count==pos) {
	if(temp==head) {
	  head=temp->next;
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
    printf("\t\tThe Node Number Is Not Found...");
    line;
  }
}

int del_middle_inf(int loc) {
  int flag=0;
  struct node *temp,*var1;
  temp=head;
  if(head==NULL) {
    line;
    printf("\t\tLinked_is_an_Empty");
    line;
  } else {
    while(temp!=NULL) {
      if(temp->data==loc) {
	if(temp==head) {
	  head=temp->next;
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
    printf("\t\tThe Value Is Not Found...");
    line;
  }
}

int display() {
  trav=head;
  if(trav==NULL) {
    line;
    printf("\t\tLinked List is Empty");
    line;
  } else {
    line;
    while(trav!=NULL) {
      printf("%d->",trav->data);
      trav=trav->next;
    }
    line;
    printf("\n");
  }
}

int update(int loc,int val) {
  int flag=0;
  struct node *temp,*var1;
  temp=head;
  while(temp!=NULL) {
    if(temp->data==loc) {
      temp->data=val;
      flag=1;
    }
    temp=temp->next;
  }
  if(flag==0) {
    line;
    printf("\t\tValue Not Found Please Try Again...");
    line;
  } else {
    line;
    printf("\t\tValue is Updated...");
    line;
  }
}

int travarse(int val) {
  int flag=0,count=1;
  struct node *temp;
  temp=head;
  if(head==NULL) {
    line;
    printf("\t\tLinked_is_an_Empty");
    line;
  } else {
    while(temp!=NULL) {
      if(temp->data==val) {
	line;
	printf("The %d Element is %d Position",val,count);
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
    printf("\t\tValue Not Found Please Try Again...");
    line;
  }
}

int main() {
  int ch,val,n,loc,pos;
  do {
    printf("\n\t\t\tSINGLY_LINKED_LIST");
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
    printf("\nEnter Your Choice: ");
    scanf("%d",&ch);
    switch(ch) {
    case 1:
      printf("\nEnter Value to Insert a node: ");
      scanf("%d",&val);
      insert_begin(val);
      display();
      break;
    case 2:
      printf("\nEnter a Location Value After You Want To Insert Val: ");
      scanf("%d",&loc);
      printf("\nEnter Value to Insert a node: ");
      scanf("%d",&val);
      insert_middle_inf(val,loc);
      display();
      break;
    case 3:
      printf("\nEnter a Position After You Want To Insert Val: ");
      scanf("%d",&pos);
      printf("\nEnter Value to Insert a node: ");
      scanf("%d",&val);
      insert_middle_num(val,pos);
      display();
      break;
    case 4:
      printf("\nEnter Value to Insert a node: ");
      scanf("%d",&val);
      insert_end(val);
      display();
      break;
    case 5:
      del_begin();
      display();
      break;
    case 6:
      printf("\nEnter a Location Value You Want To Delete Val: ");
      scanf("%d",&loc);
      del_middle_inf(loc);
      display();
      break;
    case 7:
      printf("\nEnter a Position You Want To Delete Val: ");
      scanf("%d",&pos);
      del_middle_num(pos);
      display();
      break;
    case 8:
      display();
      del_end();
      display();
      break;
    case 9:
      printf("\nEnter The Value You Want To Update: ");
      scanf("%d",&loc);
      printf("\nEnter The New Value To Update a Node Value: ");
      scanf("%d",&val);
      update(loc,val);
      display();
      break;
    case 10:
      printf("\nEnter The Value you Want To Search: ");
      scanf("%d",&val);
      travarse(val);
      display();
      break;
    case 11:
      display();
      break;
    }
  } while(ch!=12);
}
