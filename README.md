# Learning Halide for Machine Learning

This repository contains the sources for a new ML-focused Halide
tutorial, hosted at [https://people.csail.mit.edu/tzumao/tmp/learning_halide/](https://people.csail.mit.edu/tzumao/tmp/learning_halide/).

## Building the website

We use [Sphinx](https://www.sphinx-doc.org/en/master/#) to generate the web pages. First, create a virtual environment:

```
$ python3 -m venv venv
$ source venv/bin/activate
$ pip install -r requirements.txt
```

Seeing an error like "Failed building wheel for sphinx-tabs" is OK

Then you can build the website via

```
$ cd docs
$ make html
```
