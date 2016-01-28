{- Assignment 3 - Memory and Mutation

This file contains code which uses the mutation library found in Mutation.hs
-}
module MutationUser (
    pointerTest,
    swap,
    swapCycle)
    where

import Mutation (
    (>>>), get, set, def, Mutable(..), Pointer(..), Memory, StateOp (..), (>~>), Value (..), runOp, free, alloc 
    ) 
import Data.List (sortBy, intersect, nub)

-- | Takes a number <n> and memory, and stores two new values in memory:
--   - the integer (n + 3) at location 100
--   - the boolean (n > 0) at location 500
--   Return the pointer to each stored value, and the new memory.
--   You may assume these locations are not already used by the memory.
pointerTest ::Integer -> StateOp (Pointer Integer, Pointer Bool)
pointerTest n = StateOp (\s -> 
      let (p1, mem) = runOp (def 100 (n+3)) s 
          (p2, mem2)= runOp (def 500 (n>0)) mem
      in ((p1,p2), mem2))     
{-pointerTest :: Integer -> StateOp b
pointerTest n =def 100 ((n+3) :: Integer ) >~> def 500 ((n > 0) :: Bool)  
{--}
pointerTest :: Integer -> Memory -> ((Pointer Integer, Pointer Bool), Memory)
pointerTest n mem  = 
    let (pin, mem1) = def mem 100 (n + 3)
        (pbo, mem2) = def mem1 500 (n > 0) -}


swap :: Mutable a => Pointer a -> Pointer a -> StateOp a
swap p1 p2 = 
    get p1 >~> \p ->
        get p2 >~> \k -> 
            set p1 k >>> set p2 p

swapCycle :: Mutable a => [Pointer a] -> StateOp a
swapCycle (p1:rest) = 
            if (length rest /= 0)
                then swap p1 (last rest) >>> swapCycle (p1: (init rest))
                else swap p1 p1