# ProgressionSystem

## Overview

It's a Progression System that allows unlocking new characters by playing the game developed for [Bomber project](https://github.com/JanSeliv/Bomber).

It's developed as a Modular Game Feature (MGF) module. Technically, it's a plugin, but can't be used in other projects as it heavily depends on the Bomber code. However, you could still use it in your projects if you manage to replace all Bomber dependencies with your own.

## Implementation Details

![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/61930c27-6247-45b3-ac81-a349841ed12e)

Added progression system:
1. Each level contains certain amounts of locked and unlocked stars ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/d11eb40e-624b-4fa3-b4fb-51b749e405c6)
3. The amount of start dynamical can be different for different levels
4. Locked levels have a locking overlay  ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/1a81f9e1-9d26-43bf-89e3-6ab6f6725bda)
5. Settings of the progression are stored in the DataTable file.
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/559ff33f-d555-47b6-8d21-c4ccf23a1e77)
6. By default if there is no save game, progression will be initiated with the first level in the data table
7. Progression scores increase for win, lose, draw. Time to survive is not supported.
8.  Amount of starts for each end game result configured in the data table
    ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/97c6706d-025c-4006-97cb-9086a23b4eb7)

9. To restart the save go to Bomber\Saved\SaveGames and delete PSCSaveGame.sav
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/842a70f4-c509-4670-ab9c-0ce86bb8aa9e)


Required settings:
1. Data asset to be set for a BP_ProgressionSystemComponent:
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/174c8835-486d-4b9c-8d6a-3ad8c0f0ce0b)
2. Progression system uasset file configuration:
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/254ae1f3-5ca2-4524-9194-e660c3fe3b15)
3. Data asset configuration
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/feb631d7-3440-4042-8526-233d84d44d7f)
4. Main menu widget:
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/e037d9a3-d712-43de-8ef8-c4918e1e3adf)


Additional:
1. There is a saving score widget in the end game.
2. For now it is disabled but can be easily reactivated.
   ![image](https://github.com/h4rdmol/ProgressionSystem/assets/5227233/a56bc34e-5056-4c83-bca5-d2a223879cb5)

## License

This project is licensed under the terms of the MIT license. See [LICENSE](LICENSE) for more details.

We hope you find this plugin useful and we look forward to your feedback and contributions.

