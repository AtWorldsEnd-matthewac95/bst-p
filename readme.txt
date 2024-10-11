This is a C++ project I made to help me make a mod for Pokemon Red.

In Pokemon Red and Blue, each Pokemon has a unique base XP yield (with the exception of Mew and Bulbasaur, which both have a base XP yield of 64).
In other words, the base XP yield for Pokemon can be thought of as a strictly increasing value. Any list of discrete unique elements can be sorted to be a strictly increasing list.

In order to have more direct control over how much XP each Pokemon would award in my mod, I wanted to rank each Pokemon relative to each other in terms of how much XP they give; however, that's a rather daunting prospect when there are 151 Pokemon.
To make this task more managable, I envisioned utilizing some sort of binary sort to reduce the amount of questions I'd have to ask while ranking each Pokemon. For example, suppose I've already established that Weedle gives less XP than Pikachu, who in turn gives less XP than Rhyhorn. If I then say Charizard should give more XP than Rhyhorn, there would be no need for me to separately establish that Charizard also gives more XP than Weedle and Pikachu - that's already implied. On the other hand, suppose I say Pidgeotto gives LESS XP than Rhyhorn - well now I do need to ask if Pidgeotto gives more or less XP than Pikachu, but there's no need to establish it gives less than Charizard, because again that's already implied.

Once I established I wanted to use a binary sort in this way, an AVL tree seemed like the best option. I had never made an AVL tree, so I took it upon myself to learn how they work and make one.

This project is the result of that design. I hope it does a good enough job in conveying how I think and the quality of my work.