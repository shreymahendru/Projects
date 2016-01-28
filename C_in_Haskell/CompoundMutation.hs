{- Assignment 3 - Memory and Mutation

This file contains the code responsible for working with association lists,
which you will use as the data structure for storing "mutable" data.
 

-}

-- **YOU MUST ADD ALL FUNCTIONS AND TYPES TO THIS LIST AS YOU CREATE THEM!!**
module Mutation (
    Mutable, get, set, def, free, alloc,
    Memory, Pointer(..), (>>>), (>~>), runOp, StateOp(..), Value(..), returnVal
    )
    where

import AList (AList, lookupA, insertA, updateA, removeA)

-- A type representing the possible values stored in memory.
data Value = IntVal Integer |
             BoolVal Bool
             deriving Show

-- A type representing a container for stored "mutable" values.
type Memory = AList Integer Value

-- A type representing a pointer to a location in memory.
data Pointer a = P Integer | Per Integer Integer
--A type Representing a StateOP
data StateOp a = StateOp (Memory -> (a, Memory))


-- A type representing a person with two attributes:
-- age and whether they are a student or not.
data Person = Person Integer Bool deriving Show


--Test Function
personTest :: Person -> Integer -> StateOp (Integer, Bool, Person)
personTest person x =
    -- not using alloc, but we could
    def 1 person >~> \personPointer ->
    get (personPointer @@ age) >~> \oldAge ->
    set (personPointer @@ age) x >>>
    get (personPointer @@ isStudent) >~> \stu ->
    get (personPointer @@ age) >~> \newAge ->
    set personPointer (Person (2 * newAge) (not stu)) >>>
    get personPointer >~> \newPerson ->
    get (personPointer @@ isStudent) >~> \newStu ->
    returnVal (oldAge, newStu, newPerson)

-- Takes the age(int) and the isstudent(Bool) variable and returns a pointer to bool 
isStudent ::Mutable a => Integer -> Integer -> Pointer a 
isStudent ag stu = (P stu)
-- Takes the age(int) and the isstudent(Bool) variable and returns a pointer to int 
age ::Mutable a => Integer -> Integer -> Pointer a 
age ag stu =(P ag)    

--Takes in a person os the let side and an op to be performed on it (isStudent/age) 
--it returns a pointer to Bool or int (depends on the input)
(@@) :: Pointer a -> (Integer -> Integer -> Pointer b) ->  Pointer b 
(Per ag stu) @@ op = op ag stu 

--Appplies the memory to the StateOP function
runOp :: StateOp a -> Memory -> (a, Memory)
runOp (StateOp op) mem = op mem

--Then 2 StatOp functions
(>>>) :: StateOp a -> StateOp b -> StateOp b
StateOp op1 >>> StateOp op2 = StateOp (\s -> 
    let (_, mem1)= op1 s
    in op2 mem1)
--Bind 2 StateOp functions
(>~>) :: StateOp a -> (a -> StateOp b) -> StateOp b
StateOp op1 >~> op2 = StateOp(\s ->
     let (x, mem) = op1 s
         new = op2 x
    in runOp new mem)

returnVal :: a -> StateOp a
returnVal a = StateOp (\s -> (a, s))

-- Given a key and an Alist returns 1 if the key is in the alist else returns 0
inList ((current, val):rest) key =
    if (current == key) 
        then 1
        else 
            inList rest key
inList [] key = 0    

--Find any integer i that is not being used as a key in the Memeory (or find free space)
get_int list i = 
    if (inList list i) == 1
        then (get_int list (i+1)) 
    else i 

--Delete a key value pair from the memory 
free :: Mutable a => Pointer a -> StateOp ()
free (P key)= StateOp(\s -> 
   if (inList s key == 1)
      then ((),removeA s key)
      else error "Not in Memory!")       
--Finds an empty space in the memory and adds the value given 
alloc :: Mutable a => a -> StateOp (Pointer a)    
alloc a = StateOp(\s -> 
   let i = get_int s 0
    in  
     runOp (def i a) s)

 

instance Mutable Bool where
       get (P z) = StateOp (\s -> let r = lookupA s z
                                                in 
                                                case r of
                                                (BoolVal i) -> (i, s)
                                                otherwise -> error "Not a pointer to Bool" )                                               
        
       set (P z) val= StateOp (\s ->  (val, updateA s (z, (BoolVal val))))

       def int z = StateOp (\s -> ((P int), insertA s (int, (BoolVal z))))
        
                 


instance Mutable Integer where
        
        get (P z)= StateOp ( \s ->
            let r = lookupA s z
            in 
            case r of
                (IntVal i) -> (i, s)
                otherwise -> error "Not a pointer to Integer" )
       
        set (P z) val=  StateOp (\s -> (val, updateA s (z, (IntVal val)))) 

        
        def int z = StateOp (\s -> ( (P int), insertA s (int, (IntVal z))))
                                       


instance  Mutable Person where
    def int (Person a s) = StateOp(\m -> 
          let int2 = get_int m 0
              (_, mem) = runOp (def int a >>> def int2 s) m
          in  ((Per int int2), mem))


    get (Per int1 int2) = StateOp(\s ->
        let (one, _ ) = runOp (get (P int1)) s
            (two, mem) = runOp (get (P int2)) s 
          in (Person one two, mem ))

    set (Per int1 int2) (Person a s) = StateOp(\m ->
         let (p1, mem1)= runOp (set (P int1) a) m 
             (p2, mem) = runOp (set (P int2) s) mem1
            in ((Person p1 p2), mem) )     

-- Type class representing a type which can be stored in "Memory".
class Mutable a where
    

    get :: Pointer a -> StateOp a   
    -- Look up a value in memory referred to by a pointer.
 
    -- Change a value in memory referred to by a pointer.
    -- Return the new memory after the update.
    set :: Pointer a -> a -> StateOp a

    -- Create a new memory location storing a value, returning a new pointer
    -- and the new memory with the new value.
    -- Raise an error if the input Integer is already storing a value.
    def :: Integer -> a -> StateOp (Pointer a) 











