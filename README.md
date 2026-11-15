# Chunkviewer
Chunkviewer is a small toy voxel-renderer using raylib and ImGui.

<img width="960" height="540" alt="Chunkviewer-0 1" src="https://github.com/user-attachments/assets/f2f996be-aba6-4c28-84e5-904647142364" />


## Optional arguments:

### World generation:
Where "N" is a whole number:\
"--seed N"     - sets the seed for the terrain generation (1-9999). Defaults to 7.\
"--radius N"   - sets the radius of the world, in number of chunks (0-6). Defaults to 4.

### Save/Load:
Where "FILE" is the filepath:\
"--save FILE"  - saves the generated world to a new or existing ".world" file.\
(E.g. "--save test.world", "--save saves/test.world")\

"--load FILE"  - loads the world from an existing ".world" file.\
(E.g. "--load test.world", "--load saves/test.world")

### Stats:
No window is launched:\
"--stats"      - prints stats about the world, whether generated, saved or loaded.\
"--bench"      - prints measured time to generate a new world with the specified seed and radius.\
Only the world generation flags work when --bench is in use.
