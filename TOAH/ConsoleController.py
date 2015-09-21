# Copyright 2014 Dustin Wehr
# Distributed under the terms of the GNU General Public License.
#
# This file is part of Assignment 1, CSC148, Winter 2014.
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This file is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this file.  If not, see <http://www.gnu.org/licenses/>.
"""
ConsoleController: User interface for manually solving Anne Hoy's problems 
from the console.

move: Apply one move to the given model, and print any error message 
to the console. 
"""

from TOAHModel import TOAHModel, Cheese


def move(model: TOAHModel, origin: int, dest: int):
    '''
    Module method to apply one move to the given model, and print any
    error message to the console. 
    
    model - the TOAHModel that you want to modify
    origin - the stool number (indexing from 0!) of the cheese you want 
             to move
    dest - the stool number that you want to move the top cheese 
            on stool origin onto.        
    '''    
 
    model.move(origin, dest)
            
    print(model)

                 
                
                


class ConsoleController:
    
    def __init__(self: 'ConsoleController', 
                 number_of_cheeses: int, number_of_stools: int):
        """
        Initialize a new 'ConsoleController'.

        number_of_cheeses - number of cheese to tower on the first stool                            
        number_of_stools - number of stools
        """
        self.number_of_cheeses=number_of_cheeses
        self.number_of_stools=number_of_stools
        
                
    def play_loop(self: 'ConsoleController'):
        '''    
        Console-based game. 
        TODO:
        -Start by giving instructions about how to enter moves (which is up to
        you). Be sure to provide some way of exiting the game, and indicate
        that in the instructions.
        -Use python's built-in function input() to read a potential move from
        the user/player. You should print an error message if the input does
        not meet the specifications given in your instruction or if it denotes
        an invalid move (e.g. moving a cheese onto a smaller cheese).
        You can print error messages from this method and/or from
        ConsoleController.move; it's up to you.
        -After each valid move, use the method TOAHModel.__str__ that we've 
        provided to print a representation of the current state of the game.
        '''
        
        m = TOAHModel(self.number_of_stools)
        m.fill_first_stool(self.number_of_cheeses)        
        print("Then enter 2 integers sperated by a comma. first integer will be the stool to move the cheese from and second will be the stool the cheese has to be moved")
        print("Enter 'exit' to exit")
        
       
        s = input("Enter your move: ")
        while (s != "exit"):
            l=s.split(',')
            if len(l)==2:
                if not (l[0].isalpha() and l[1].isalpha()):
                    move(m, int(l[0]),(int(l[1]))) # ask ta
                else:
                    raise IllegalInputError("Not valid input")
                
            else:
                raise IllegalInputError("Not valid input") 
                    
            s=input("Enter the next move: ")
             
        


class IllegalInputError(Exception):
    pass 

if __name__ == '__main__':
    # TODO: 
    # You should initiate game play here. Your game should be playable by
    # running this file.    
    h = input("Enter number of stools: ")
    k = input("Enter number of cheeses: ")
    if (k.isdigit() and h.isdigit()):
        m = ConsoleController(int(k),int(h))
        m.play_loop()
    else:
        raise IllegalInputError("Not valid input")    