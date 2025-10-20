

Ring Buffer is a simple Data strcuture with a fancy name : 
- what it does is : 
  - it defines an Array of Length N, 
  - it defines n spaces in the left and n spaces in the right , 
  - those spaces are empty slots .
  - and the data is stored between the Head and the tail .
  - What if you want to remove an element at the head ? 
    - simple just increase the head pointer by one , and empty that slot 
  - same for the tail .

  - Usage ?: it gives us O(1) push and pop 
    - its often used in systems that requires FIFO .
  
  - Trick ?: its named Ring Buffers , they maintain order,so when they are filled they start to overwrite old data . 
  - We Can Use Ring Buffers when it comes to Handeling Client Incomming messages streams . 
  - 