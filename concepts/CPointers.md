
Pointers Are Only Scary Because They look that way : 

    - Accessing Struct Memebers : 
        if you are willing to acces an memeber of struct, using the struct itself .
        you need to use in this case the "." dot .

        if you are willing to acces an memeber by the pointer , and not the the object itself . 
        in this case you should use the "->" arrow not the dot . 

    - Arrays and pointers : 
        an array is a contigous chunck of memory holding elements from the same type . 
        a pointer is a variable that holds the address to the first element  .
 
        the array is a pointer itself .

    - Pointers Arithmetiques : 
        pointer++ , really differs from a type to another .
        if you do int pointer , and pointer++ it will increase by 4 Bytes be cause it points to an object of 4 Bytes (int) 

        so it means that if you try to increment an pointer to an object , it will jump according to that object size .

    - Pointers To Pointers : 
        example :we want to display "hello world" using two pointers : 
        
                char *p = "Hello World" ;  
                char **p1 = &p ; 
                printf(" %s" , *p1 ) ; 

    - Difference between unisialized pointer AND an null Pointer  :
        unsialized pointer points into an random adress 
        but Null pointer means that it does not point to anything , it points into NULL 


    - VOID Pointer :
        void pointers they will seem to you that they are useless , but if you do any type of 
        memory management , you will discover this thing Called VOID POINTERS . 

        void pointers basicly points to an memory address but the difference is that it does not know 
        what lives there (struct , integer , double) .
        
        the block of memory needs to be casted and used , example : 
        
        void * MyAlloc(size_t cbSize) 
        {
            return malloc(cbSize); 
        }
        
        void foo()
        {
            House * pHouse = (House *) MyAlloc(sizeof(House)); 
            
        }
        

    - Function Pointer : 
        it acts the same as normal pointer that point to a chuck of memory , 
        but this time it points into a chunk of code not data . 

        so this basicly let us to dynamicly change the function called in our program 
        example : 
        
            int add(int a , int b ) 
            {
                return a + b ; 
            }

            int (*pAdd)(int,int) = add  ; 
            int sum = (*pAdd)(5,3) 

        
        
