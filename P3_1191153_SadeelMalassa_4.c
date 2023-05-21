#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//SadeelMalassa_1191153_4

typedef struct AVLnode *AVLNode;

struct AVLnode
{
    AVLNode Left;
    AVLNode Right;
    int Height; //Balance information
    char Name[250];//patient info
    char gender[2];
    char Adate[100];
    char Bdate[9];
    char illness[250];
    char address[250];
    char bloodT[2];

};

typedef struct hash //hash struct
{
    AVLNode P;
    int visit;
    int del;

} hash_table;

int TableSizeG; //global Table size
int n;
int capacity; //capacity of the hash table
AVLNode Find( char NameF[400], AVLNode T );     //functions used
void lexicographic(AVLNode P);
void SameIllness(char NameI[400],AVLNode T);
void Save(FILE * out,AVLNode P);
AVLNode DeleteP(char Name[400],AVLNode P);
int validDate(char num[400]);
int validGender(char G[2]);
int size(AVLNode S);
int Hash_TableSize(int sizeA);
int arrIndex(char* key,int TableSize);
hash_table* insertToHash(AVLNode P,hash_table* HT,int TableSize);
void print_table(hash_table* HT,int TableSize);
void rehash(hash_table* HT );
void search_Hash(hash_table* HT,char name[400],int TableSize);
int hashCapacity(hash_table* HT,int TableSize);
void Delete_hash(hash_table* HT,char name[400],int TableSize);
void SaveHahToFile(FILE *out,hash_table* HT,int TableSize);
hash_table* insertToHash2(AVLNode P,hash_table* HT,int TableSize);
hash_table* insertToHash3(AVLNode P,hash_table* HT,int TableSize);

AVLNode MakeEmpty( AVLNode T )
{
    if( T != NULL )
    {
        MakeEmpty( T->Left);
        MakeEmpty( T->Right );
        free( T );
    }
    return NULL;
}

AVLNode FindMin( AVLNode T )
{
    if( T == NULL )
        return NULL;
    else if( T->Left== NULL )
        return T;
    else
        return FindMin( T->Left);
}

AVLNode FindMax( AVLNode T )
{
    if( T != NULL )
        while( T->Right != NULL )
            T = T->Right;

    return T;
}

int Height( AVLNode P )
{
    if( P == NULL )
        return -1;
    else
        return P->Height;
}

int getBalance(AVLNode T)  //check tree palance
{
    if (T == NULL)
        return 0;
    return Height(T->Left) - Height(T->Right);
}

int Max( int Lhs, int Rhs )
{
    return Lhs > Rhs ? Lhs : Rhs;
}

AVLNode SingleRotateWithLeft( AVLNode K2 )
{
    AVLNode  K1;

    K1 = K2->Left;
    K2->Left= K1->Right;
    K1->Right = K2;

    K2->Height = Max( Height( K2->Left), Height( K2->Right ) ) + 1;
    K1->Height = Max( Height( K1->Left), K2->Height ) + 1;

    return K1;
}


AVLNode SingleRotateWithRight( AVLNode K1 )
{
    AVLNode K2;

    K2 = K1->Right;
    K1->Right = K2->Left;
    K2->Left= K1;

    K1->Height = Max( Height( K1->Left), Height( K1->Right ) ) + 1;
    K2->Height = Max( Height( K2->Right ), K1->Height ) + 1;

    return K2;
}



AVLNode DoubleRotateWithLeft( AVLNode K3 )
{

    K3->Left= SingleRotateWithRight( K3->Left);

    return SingleRotateWithLeft( K3 );
}

AVLNode DoubleRotateWithRight( AVLNode K1 )
{

    K1->Right = SingleRotateWithLeft( K1->Right );

    return SingleRotateWithRight( K1 );
}


AVLNode Insert(char insertion[8][400], AVLNode T)
{

    if( T == NULL )
    {

        T = malloc( sizeof( struct AVLnode ) );
        if( T == NULL )
        {
            printf( "Out of space!!!" );
        }
        else
        {
            strcpy(T->Name,insertion[0]);
            strcpy(T->gender,insertion[1]);
            strcpy(T->Adate,insertion[2]);
            strcpy(T->Bdate,insertion[3]);
            strcpy(T->illness,insertion[4]);
            strcpy(T->address,insertion[5]);
            strcpy(T->bloodT,insertion[6]);
            T->Height = 0;
            T->Left= T->Right = NULL;
        }

    }
    else if((strcmp(insertion[0],T->Name))<0)
    {
        T->Left= Insert(insertion, T->Left);
        if( Height( T->Left) - Height( T->Right ) == 2 )
        {
            if( strcmp(insertion[0],T->Left->Name)<0)
                T = SingleRotateWithLeft( T );
            else
                T = DoubleRotateWithLeft( T );
        }
    }
    else if((strcmp(insertion[0],T->Name))>0)
    {
        T->Right = Insert( insertion, T->Right );
        if( Height( T->Right ) - Height( T->Left) == 2 )
        {
            if((strcmp(insertion[0],T->Right->Name))>0 )
                T = SingleRotateWithRight( T );
            else
                T = DoubleRotateWithRight( T );
        }
    }

    T->Height = Max( Height( T->Left), Height( T->Right ) ) + 1;
    return T;
}

void PrintInOrder( AVLNode t)
{
    if( t != NULL)
    {
        PrintInOrder( t->Left);
        printf("\n");
        printf("%s\t", t->Name);
        printf("%s\t", t->gender);
        printf("%s\t", t->Adate);
        printf("%s\t", t->Bdate);
        printf("%s\t", t->illness);
        printf("%s\t", t->address);
        printf("%s\t", t->bloodT);
        PrintInOrder( t->Right );

    }
}

hash_table* create(int TableSize) // create new hash table
{

    hash_table *HT = (hash_table*)malloc(sizeof(struct hash)*TableSize) ; // create the table
    for(int i=0; i<TableSize ; i++ ) //assign null values
    {
        HT[i].P=NULL;
        HT[i].visit = 0 ;
        HT[i].del = 0 ;
    }
    return HT ;
}

int n;

int main()
{
    FILE *in = fopen("patients.txt","r");
    FILE* out = fopen("patients_hash.data","w");
    char data[400];
    char* token;
    char insertion[8][400];
    int i=0;

    AVLNode patients = NULL; //AVL tree

    while(!feof(in)) //read from file and divide the data according to the "#" sign
    {

        fgets(data,400,in);
        token = strtok(data, "#");
        i=0;
        while (token != NULL)
        {
            strcpy(insertion[i],token);
            token = strtok(NULL, "#");
            i++;

        }
        patients = Insert(insertion,patients); //insert to patients tree

    }


    int choice;
    char NameF[400];
    char NameD[400];
    int s,g;



    int y=1;
    while(y==1) //AVL menu
    {
        printf("\n");
        printf(" 1. Add new patients\n");
        printf(" 2. Find patients\n");
        printf(" 3. List all patients in lexicographic order\n");
        printf(" 4. List all patients that have the same illness.\n");
        printf(" 5. Delete.\n");
        printf(" 6. Save to file.\n");
        printf(" 7. EXIT AND MOVE TO HASH MENU.\n");
        printf("enter choice : ");
        scanf(" %d", &choice);
        printf("\n");

        switch(choice)
        {

        case 1: //take the new data add them to insertion array then insert the array to the tree
        {
            int n=0;
            printf("patient's Name : ");
            scanf("%s[^\n]",insertion[n]);
            n++;

            printf("patient's gender : ");
            scanf("%s[^\n]",insertion[n]);
            g=validGender(insertion[n]);
            while(g==0)
            {
                printf("Value not valid, please insert F : FEMALE / M : MALE \n ");
                printf("patient's gender : ");
                scanf("%s[^\n]",insertion[n]);
                g=validGender(insertion[n]); //check if the gender value is valid
            }
            n++;

            printf("patient's Admission Date : ");
            scanf("%s[^\n]",insertion[n]);
            s=validDate(insertion[n]);
            while(s==0)
            {
                printf("Value not valid, please try again\n ");
                printf("patient's Admission Date : ");
                scanf("%s[^\n]",insertion[n]);
                s=validDate(insertion[n]); //check if the Date is numeric
            }
            n++;

            printf("patient's Birth Date : ");
            scanf("%s[^\n]",insertion[n]);
            s=validDate(insertion[n]);
            while(s==0)
            {
                printf("Value not valid, please try again\n ");
                printf("patient's Admission Date : ");
                scanf("%s[^\n]",insertion[n]);
                s=validDate(insertion[n]); //check if the Date is numeric
            }
            n++;

            printf("patient's Illness : ");
            scanf("%s[^\n]",insertion[n]);
            n++;

            printf("patient's Address : ");
            scanf("%s[^\n]",insertion[n]);
            n++;

            printf("patient's Blood Type : ");
            scanf("%s[^\n]",insertion[n]);
            n++;

            patients = Insert(insertion,patients);

            break;
        }

        case 2:
        {
            printf("Enter patient's Name : ");
            gets(NameF);
            gets(NameF);
            Find(NameF,patients);
            break;

        }
        case 3:
        {
            PrintInOrder(patients);
            break;
        }
        case 4:
        {
            printf("Insert Illness Name : ");
            gets(NameF);
            gets(NameF);
            SameIllness(NameF,patients);
            break;
        }
        case 5:
        {
            printf("Enter patient's Name to delete: ");
            gets(NameD);
            gets(NameD);
            DeleteP(NameD,patients);
            break;

        }
        case 6:
        {
            Save(out,patients);
            fclose(out);
            break;
        }
        case 7:
        {
            y=2;
            break;
        }

        }
    }



    AVLNode Record = NULL;
    char insertH[8][400];
    int sizeA = size(patients);
    TableSizeG = Hash_TableSize(sizeA);
    hash_table* HT= create(TableSizeG);
    insertToHash(patients,HT,TableSizeG);


    y=1;

    while(y==1)  //Hash menu
    {
        printf("\n");
        printf(" 1. print Hash Table\n");
        printf(" 2. print Table Size\n");
        printf(" 3. print the used Hash function\n");
        printf(" 4. Insert new record\n");
        printf(" 5. Search By Name\n");
        printf(" 6. DELETE RECORD !\n");
        printf(" 7. Save Hash Table to File.\n");

        printf("Enter choice : ");
        scanf(" %d", &choice);
        printf("\n");

        switch(choice)
        {
        case 1:
        {
            print_table(HT,TableSizeG); //print Hash table
            break;
        }
        case 2:
        {
            printf("Hash Table Size = %d ",TableSizeG); //print Table size
            break;
        }
        case 3:
        {
            printf("Linear : index = hashValue+i %% TableSize; / 0 <= i < TableSize ");
            break;
        }
        case 4:         //add new record to hash
        {
            printf("insert the following data : \n");
            int n=0;
            printf("patient's Name : ");
            scanf("%s[^\n]",insertH[n]);
            n++;

            printf("patient's gender : ");
            scanf("%s[^\n]",insertH[n]);
            g=validGender(insertH[n]);
            while(g==0)
            {
                printf("Value not valid, please insert F : FEMALE / M : MALE \n ");
                printf("patient's gender : ");
                scanf("%s[^\n]",insertH[n]);
                g=validGender(insertH[n]);
            }
            n++;

            printf("patient's Admission Date : ");
            scanf("%s[^\n]",insertH[n]);
            s=validDate(insertH[n]);
            while(s==0)
            {
                printf("Value not valid, please try again\n ");
                printf("patient's Admission Date : ");
                scanf("%s[^\n]",insertH[n]);
                s=validDate(insertH[n]);
            }
            n++;

            printf("patient's Birth Date : ");
            scanf("%s[^\n]",insertH[n]);
            s=validDate(insertH[n]);
            while(s==0)
            {
                printf("Value not valid, please try again\n ");
                printf("patient's Admission Date : ");
                scanf("%s[^\n]",insertH[n]);
                s=validDate(insertH[n]);
            }
            n++;

            printf("patient's Illness : ");
            scanf("%s[^\n]",insertH[n]);
            n++;

            printf("patient's Address : ");
            scanf("%s[^\n]",insertH[n]);
            n++;

            printf("patient's Blood Type : ");
            scanf("%s[^\n]",insertH[n]);
            n++;

            Record = Insert(insertH,Record);
            insertToHash3(Record,HT,TableSizeG);
            break;
        }
        case 5: //search in hash table
        {
            printf("Enter patient's Name : ");
            gets(NameF);
            gets(NameF);
            search_Hash(HT,NameF,TableSizeG);
            break;
        }
        case 6: //delete record
        {
            printf("Enter patient's Name : ");
            gets(NameF);
            gets(NameF);
            Delete_hash(HT,NameF,TableSizeG);
            break;
        }
        case 7: //print Hash table to file
        {
            SaveHahToFile(out,HT,TableSizeG);
            fclose(out);
            break;
        }

        }
    }



    return 0;
}
AVLNode Find( char NameF[400], AVLNode T ) //find patient by name and update info if user asked
{
    int x;
    int update;
    char NewV[400];
    int y=1;

    if( T == NULL )
    {
        return NULL;
    }
    if((strcmp(NameF,T->Name))<0 )
    {
        return Find( NameF, T->Left);
    }
    else if((strcmp(NameF,T->Name))>0 )
    {
        return Find( NameF, T->Right );
    }
    else //print all info then ask if the user wants to update them
    {

        printf("Name : %s , Gender : %s , Birth Date : %s , Illness : %s ,Admission Date : %s , Address : %s ,Blood Type : %s \n",T->Name,T->gender,T->Bdate,T->illness,T->Adate,T->address,T->bloodT);
        printf("Update Information ? ");
        printf(" Yes: Enter 1\tNo: Enter 0\n");
        scanf("%d", &x);
        if(x==1)
        {
            printf("pick !");
            while(y==1)
            {
                printf("\n");
                printf(" 1. update Name\n");
                printf(" 2. update Gender\n");
                printf(" 3. update Admission Date\n");
                printf(" 4. update Birth Date\n");
                printf(" 5. update Illness\n");
                printf(" 6. update Address\n");
                printf(" 7. update Blood Type\n");
                printf(" 8. Exit\n");
                printf("enter Number to modify : ");
                scanf(" %d", &update);
                printf("\n");

                switch(update)
                {
                case 1: //when we update the name (key) we delete all the node and insert it again to save the balance
                {
                    printf("New Name : ");
                    gets(NewV);
                    gets(NewV);
                    char replace[8][400];
                    strcpy(replace[0],NewV);
                    strcpy(replace[1],T->gender);
                    strcpy(replace[2],T->Adate);
                    strcpy(replace[3],T->Bdate);
                    strcpy(replace[4],T->illness);
                    strcpy(replace[5],T->address);
                    strcpy(replace[6],T->bloodT);
                    Insert(replace,T);
                    DeleteP(T->Name,T);
                    printf("%s ", T->Name);
                    break;
                }
                case 2: //other value only change the value asked for it self using strcpy
                {
                    printf("New Gender : ");
                    gets(NewV);
                    gets(NewV);
                    strcpy(T->gender,NewV);
                    printf("%s ", T->gender);
                    break;
                }
                case 3:
                {
                    printf("New Admission Date : ");
                    gets(NewV);
                    gets(NewV);
                    strcpy(T->Adate,NewV);
                    printf("%s ", T->Adate);
                    break;
                }
                case 4:
                {
                    printf("New Birth Date : ");
                    gets(NewV);
                    gets(NewV);
                    strcpy(T->Bdate,NewV);
                    printf("%s ", T->Bdate);
                    break;
                }
                case 5:
                {
                    printf("New Illness : ");
                    gets(NewV);
                    gets(NewV);
                    strcpy(T->illness,NewV);
                    printf("%s ", T->illness);
                    break;
                }
                case 6:
                {
                    printf("New Address : ");
                    gets(NewV);
                    gets(NewV);
                    strcpy(T->address,NewV);
                    printf("%s ", T->address);
                    break;
                }
                case 7:
                {
                    printf("New Blood Type : ");
                    gets(NewV);
                    gets(NewV);
                    strcpy(T->bloodT,NewV);
                    printf("%s ", T->bloodT);
                    break;
                }
                case 8:
                {
                    y=2;
                }

                }

            }

        }
        return T;
        printf("NONE FUOND!\n");
    }
}

void SameIllness(char NameI[400],AVLNode t) //find patient with same illness then print their info
{
    if( t != NULL)
    {
        if((strcmp(NameI,t->illness))==0)
        {
            printf("%s\t", t->Name);
            printf("%s\t", t->gender);
            printf("%s\t", t->Adate);
            printf("%s\t", t->Bdate);
            printf("%s\t", t->illness);
            printf("%s\t", t->address);
            printf("%s\t", t->bloodT);
            printf("\n");
        }
        SameIllness(NameI,t->Left);
        SameIllness(NameI,t->Right);
    }

}

void Save(FILE * out,AVLNode P) //save AVL tree to file
{

    if( P != NULL)
    {
        Save(out, P->Left);
        fprintf(out,"%s\t", P->Name);
        fprintf(out,"%s\t", P->gender);
        fprintf(out,"%s\t", P->Adate);
        fprintf(out,"%s\t", P->Bdate);
        fprintf(out,"%s\t", P->illness);
        fprintf(out,"%s\t", P->address);
        fprintf(out,"%s\t", P->bloodT);
        fprintf(out,"\n");
        Save(out, P->Right );
    }


}

AVLNode DeleteP(char Name[400],AVLNode P) //Delete function that Finds the node neede by name the delete it and re balance the tree
{

    if (P == NULL)
        return P;


    if  ((strcmp(Name,P->Name))<0)
        P->Left= DeleteP(Name,P->Left);

    else if((strcmp(Name,P->Name))>0)
        P->Right = DeleteP(Name,P->Right);

    else
    {
        if( (P->Left== NULL) || (P->Right == NULL) )
        {
            AVLNode D = P->Left? P->Left: P->Right;

            if (D == NULL)
            {
                D = P;
                P = NULL;
            }
            else
                *P = *D;

            free(D);
        }
        else
        {
            AVLNode D = FindMin(P->Right);
            strcpy(D->Name,P->Name);
            P->Right = DeleteP(D->Name,P->Right);
        }
    }

    if (P == NULL)
        return P;

    P->Height = 1 + Max(Height(P->Left),Height(P->Right));

    int balance = getBalance(P);

    if (balance > 1 && getBalance(P->Left) >= 0)
        return SingleRotateWithRight(P);

    if (balance > 1 && getBalance(P->Left) < 0)
    {
        return DoubleRotateWithLeft(P);
    }

    if (balance < -1 && getBalance(P->Right) <= 0)
        return SingleRotateWithLeft(P);


    if (balance < -1 && getBalance(P->Right) > 0)
    {
        return SingleRotateWithRight(P);
    }

    return P;
}

int validDate(char num[400]) //check if all values entered are numeric
{
    int s;
    for(int i=0; i<strlen(num); i++)
    {
        if(num[i]>='0'&&num[i]<='9')
        {
            s=1;
        }
        else
        {
            s=0;
            break;
        }

    }
    return s;
}


int validGender(char G[2]) //check if entered only M : male / F : female
{

    int s;

    if((strcmp(strlwr(G),"f"))!=0 && (strcmp(strlwr(G),"m"))!=0)
    {
        s=0;
    }
    else
    {
        s=1;
    }

    return s;

}
int size(AVLNode S) //finds the size of an AVL tree
{
    if (S==NULL)
        return 0;
    else
        return(size(S->Left) + 1 + size(S->Right));
}

int Hash_TableSize(int sizeA) //finds the size of the hash table (the prime number) bigger than the applied law
{

    int IFprime;
    int prime = sizeA*2+1;
    while(1)
    {
        IFprime =1;
        for(int i=2; i<=sqrt(prime); i++)
        {
            if(prime%i==0)
            {
                IFprime=0;
                break;
            }
        }
        if (IFprime ==1)
        {
            return prime;
        }
        prime++;

    }


}

int arrIndex(char* key, int TableSize) //finds the numeric value of a string in order to find the index (hash value)
{
    int hashValue = 0;
    while( *key != '\0' )
    {
        hashValue = ( hashValue << 5 ) + *key++;
    }
    return (hashValue);
}


hash_table* insertToHash(AVLNode P,hash_table* HT,int TableSize) //it add all the values on an AVL tree to a Hash table
{


    int index;
    double load;

    if(P!=NULL)
    {

        insertToHash(P->Left,HT,TableSize);
        load = ((double)capacity/(double)TableSize); //load factor law

        if (load<0.7) //check if reach the load factor
        {

            for(int i=0; i<TableSize; i++) //check if the index visited
            {
                index = (arrIndex(P->Name,TableSize)+i)%TableSize;
                if(HT[index].visit==0)
                {
                    HT[index].visit=1;
                    HT[index].P=P;
                    capacity++;
                    break;
                }


            }


        }

        insertToHash(P->Right,HT,TableSize);
    }

    return HT;

}

hash_table* insertToHash3(AVLNode P,hash_table* HT,int TableSize) //add values in a node to hash table
{


    int index;
    double load;

    if(P!=NULL)
    {

        load = ((double)capacity/(double)TableSize);

        if (load<0.7)
        {

            for(int i=0; i<TableSize; i++)
            {
                index = (arrIndex(P->Name,TableSize)+i)%TableSize;
                if(HT[index].visit==0)
                {
                    HT[index].visit=1;
                    HT[index].P=P;
                    capacity++;
                    break;
                }
            }
        }
        else
        {
            rehash(HT);
            printf("here!\n");

        }

    }

    return HT;

}

hash_table* insertToHash2(AVLNode P,hash_table* HT,int TableSize) //used in the rehash function
{

    int index;

            for(int i=0; i<TableSize; i++)
            {
                index = (arrIndex(P->Name,TableSize)+i)%TableSize;
                if(HT[index].visit==0)
                {
                    HT[index].visit=1;
                    HT[index].P=P;
                    capacity++;
                    break;
                }


            }
    return HT;

}


void print_table(hash_table* HT,int TableSize) // print the table
{
    for(int i =0 ; i < TableSize ; i++)
    {
        if(HT[i].visit==1) // if the cell contain values
            printf("HT[%d] :: Name: %s :: Gender: %s :: Admission Date: %s :: Birth Date: %s :: Illness: %s :: Address: %s :: Blood Type: %s\n",i,HT[i].P->Name,HT[i].P->gender,HT[i].P->Adate,HT[i].P->Bdate,HT[i].P->illness,HT[i].P->address,HT[i].P->bloodT);

        else
        {
            printf("HT[%d] :: (empty)  :: vis ->%d :: del->%d\n",i, HT[i].visit, HT[i].del );
        }
    }
}

void rehash(hash_table* HT) // rehash function creates a bigger hash table then add values to it when the load of the first table is over 0.7
{
    capacity = 0;
    int Rsize = Hash_TableSize(TableSizeG);
     hash_table *RT = create(Rsize);

    for(int i = 0 ; i<TableSizeG ; i++)
    {
        if(HT[i].visit ==1 && HT[i].del == 0 )
        {
            insertToHash2(HT[i].P,RT,Rsize);
        }
    }

    HT = RT;
    TableSizeG = Rsize;
}

void search_Hash(hash_table* HT,char name[400],int TableSize) //search for value in hash table
{
    int index;
    for(int i=0; i<TableSize; i++)
    {
        index = ((arrIndex(name,TableSize))+i)%TableSize;
        if(HT[index].visit==1)
        {
            if((strcmp(HT[index].P->Name,name))==0)
            {
                printf("Name: %s :: Gender: %s :: Admission Date: %s :: Birth Date: %s :: Illness: %s :: Address: %s :: Blood Type: %s\n",HT[index].P->Name,HT[index].P->gender,HT[index].P->Adate,HT[index].P->Bdate,HT[index].P->illness,HT[index].P->address,HT[index].P->bloodT);
                break;
            }


        }
    }
}

void Delete_hash(hash_table* HT,char name[400],int TableSize) //search for value then delete it
{
    int index;

    for(int i=0; i<TableSize; i++)
    {
        index = ((arrIndex(name,TableSize))+i)%TableSize;
        if(HT[index].visit==1)
        {
            if((strcmp(HT[index].P->Name,name))==0)
            {
                printf("found!");
                HT[index].P = NULL;
                HT[index].visit = 0;
                HT[index].del = 1;
                break;
            }
        }
    }

}

int hashCapacity(hash_table* HT,int TableSize) //finds the capacity of hash table
{
    int count=0;
    for(int i=0; i<TableSize; i++)
    {
        if(HT[i].visit == -1)
        {
            count++;
        }
    }
    return count;
}


void SaveHahToFile(FILE *out,hash_table* HT,int TableSize) //save hash table to file
{
    for(int i =0 ; i < TableSize ; i++)
    {
        if(HT[i].visit==1)
        {
            fprintf(out,"HT[%d] :: Name: %s :: Gender: %s :: Admission Date: %s :: Birth Date: %s :: Illness: %s :: Address: %s :: Blood Type: %s\n",i,HT[i].P->Name,HT[i].P->gender,HT[i].P->Adate,HT[i].P->Bdate,HT[i].P->illness,HT[i].P->address,HT[i].P->bloodT);
        }
        else
        {
            fprintf(out,"HT[%d] :: (empty)  :: vis ->%d :: del->%d\n",i, HT[i].visit, HT[i].del );
        }
    }

}
