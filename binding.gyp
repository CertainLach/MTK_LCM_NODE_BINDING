{
  "targets": [
    {
      "target_name": "module",
      "sources": [ "module.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [
        "-lbcm2835" 
      ]
    }
  ]
}