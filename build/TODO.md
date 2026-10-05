

Fix:
    - Maybe let them walk at you like stupid and then if they walk into a wall for some time then they pathmake

Features:
    - proper file management for maps (name input)
         
Long Term:
    - Cinematic view/Scripted events
    - Proper map loading
    - Saving
    - UI 
    - NPC, Questlines
    - Asset manager - map should have a asset enum which tells it what to loadmak
    - Audio


Important Notes/Plans for future structure:
    - Instead of having Inventory in GameState, I add them to the future Player/NPC struct, and the inventory loads as part of the NPC, Not on-interact
    - For shops, Inventory should maybe have a InventoryData where INVENTORY_TYPE/Dimensions/CellCount/Items(maybe...) are all located, this way it can be used for other things

Ideas:
    - Fulghor boss, which run through the arena casting holy light beneath him



Tomorry:
    - Need to fix a better system for skills, having some parts of the system split and some together is gonna get confusing

