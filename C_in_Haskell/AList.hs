{- Assignment 3 - Memory and Mutation

This file contains the code responsible for working with association lists,
which you will use as the data structure for storing "mutable" data.
-}
module AList (
    AList,
    lookupA,
    insertA,
    updateA,
    removeA
    )
    where


type AList a b = [(a, b)]

-- | Returns the value in the association list corresponding to the given key.
--   Assumes that the key is in the association list.
lookupA :: Eq a => AList a b -> a -> b
lookupA ((current, val):rest) key =
	if (current == key)
		then val
		else
			lookupA rest key

-- Given a key and an Alist returns 1 if the key is in the alist else returns 0
inList ::Eq a => AList a b -> a -> Integer 
inList ((current, val):rest) key =
	if (current == key) 
		then 1
		else 
			inList rest key
inList [] key = 0  		

-- | Returns a new association list which is the old one, except with 
--   the new key-value pair inserted. However, it returns the *same* list
--   if the key already exists in the list.
insertA :: Eq a => AList a b -> (a, b) -> AList a b
insertA lst (key, val) = 
	(key, val):  (filter (\(a, b)-> a /= key) lst)
 -- | Returns a new association list which is the old one, except with 
--   the value corresponding to the given key changed to the given new value.
--   However, it returns the *same* list if the key doesn't appear in the list.
updateA :: Eq a => AList a b -> (a, b) -> AList a b
updateA alist (key, val) = 
	if (inList alist key == 1  )
		then (key, val) : (filter (\(a, b) -> a /= key) alist)
		else 
			alist 
-- Given an Alist and a Key it removes the key and value of the Alist and 
--returns a new alist without that key value pair if key is not present returns the original Alist 
removeA :: Eq a => AList a b -> a -> AList a b
removeA ((current, val):rest) a = if (current == a)
   then removeA rest a
   else (current, val):removeA rest a

removeA [] a = []     
