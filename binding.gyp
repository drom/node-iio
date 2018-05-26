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
      ],
    }
  ]
}
