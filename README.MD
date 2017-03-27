# Hello
This is a node.js library to interract with china lcm modules

# Usage:
- Edit lcm_table.h to your lcm (You can get your table with this tool: [LK.BIN-Parser](https://github.com/Creeplays/LK.BIN-Parser))
- Attach lcm to RPI
- Build module via `node-gyp rebuild`
- Write code
- Run `sudo node yourScript.md`

# Example:
```js
const lcm=require('/folder/with/index.js')
lcm.fillScreenRGB(0,0,0);
lcm.setRotation(270);
lcm.fillAreaRGB(0,0,lcm.getWidth()/2,lcm.getHeight(),225,0,255); //Fill half of screen with purple
```

# API:
Look at `module.cc`