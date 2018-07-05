{
  "targets": [
    {
      "target_name": "iio",
      "sources": [ "src/binding.cc" ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "libraries": [
        "-liio"
      ]
    },
    {
      "target_name": "iioc",
      "sources": [ "src/iio.c" ],
      "libraries": [
        "-liio"
      ]
    }
  ]
}
