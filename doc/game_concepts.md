- Debuff System
  - Types - Defined by :
    - POISON
      - EACH X TURNS - X >= 1 - DECREASES DMG BY TURN (??)
    - TOXIC  (?)
      - EACH TURN PLUS MODIFIER
    - FROZEN (?)
      - LOSES TURN / ROLL A DICE(???)
    - STAMINA_DEBUFF (????) - SLOW/TURN/LIFE_PENALTIES
    - BURN   (?)
      - PENALTIES ATTACK STATUS - DIVIDES ATK CARD VALUES BY MODIFIER
    - PARALYZE (?)
      - INCREASES CARD ENERGY COST / ROLL A DICE(???)
    

- Card System
  - Types - Defined by
    - ATTACK:
    - BLOCK
      - WORKS LIKE EXTRA LIFE
    - DEFENSE (?)
      - DECREASES ENEMY PHYSICAL ATTACK BY VALUE
    - MAGIC
      - APPLY CARD VALUE TO SELECTED ENEMIES OR ARBRITRARIE (ALLOFEM, SORTOF)
      - CHANNELING CARDS (?) - WAIT BEFORE APPLY EFFECT | PLAY AT SOME ORDER(FIRST|LAST|AFTER|COND)
        - (?)Draw while waiting
        - (?)Discard while waiting
        - COND - [AFTER some card, (NOT) HAVING ANY, HAVING ONLY...]
      - BOARD EFFECT CARDS
        - EARTHQUAKE - APPLIES ON ENEMY PARTY (?) SINGLE/MULTIPLE TURN DAMAGE (?)  
        - DEBUFFS (?)
    - ARTIFACTS(?)

 - DECK System - ALREADY X FOR WHOLE DECK , Y FOR DISCARD PILE, Z FOR HAND
  - Deck Preset
    - Initial (default)
    - Player Conf - Uses personal setup + new cards purchased at shop
    - Max Card Qty by Type + Subtype - DEBUF+POISON >= 4 ? NOT_ALLOWED : OK 
  - DrawPile
  - DiscardPile
  - Hand
    Functions:
      - HOLD X CARDS on hand (?) - Side effects for balancing (?)
      - Draw cards while in player's turn
  Functions
    - Shuffle DrawPile(?)
      After no avaliable cards to fill players hand
      |-> shuffle discardpile and left drawpile cards
    - Swap
      |-> Hand replacemants and sorting
    - Sort
      |-> By alpha order - after discard
    - Discard
      |-> On Use | On condition

 - SHOP System - Works on player wallet (Gold amount)
   - Upgrade card 
     - Atk - Increases Card Atk value
     - Blk - Increases Player Def value
   - Buy new non-deck card
     - Reroll card poll

 - Dungeon System
   - Dungeon lvl progression
     - Monster HP  - Increased (MULTIPLIED by Rate)
     - Monster ATK - Increased (MULTIPLIED by Rate)
     - Monster Debuff (?)
     - Gold Reward - Increased by fixed amount  + Dungeon lvl modifier

     
 - Battle System
   - Turn
     - Player  - Plays Card by type - Rest: Energy cap
     - Monster - Plays one of possible own actions by rand sort
     - TODO: Player monster selection
     - Turn skip system - Skips the turn or X (same side) turns Sequentially 