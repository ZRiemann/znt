# znt
node topology 

## Table of contents
* [License](#license)
* [Usage](#usage)
  * [Welcom to znt](#welcome-to-znt)
  * [Building](#building)
    * [Linux](#linux)
    * [Windows](#windows)
  * [Interface](#interface)
* [Enjoy znt](#enjoy-znt)

## License

> MIT License
>
> Copyright (c) 2018 Z.Riemann
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:

> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

## Usage

### Welcom to znt

znt(Net Topology) aims to be the distributed system framework.

> +--------------------------------------------+<br/>
> |\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ application\ \ \ \ \ \ \ \ \ \ \ \ \ \ \ \ |<br/>
> +--------------------------------------------+<br/>
> |\ \ \ \ \ \|\ application interface layer\ |\ \ \ \|\ \ \ \ \|<br/>
> |\ \ \ \ \ \|-----------------------------+\ \ \ \|\ \ \ \ \|<br/>
> |\ znt\ \|\ \ \ \ \ \node topology layer\ \ \ \ \ \ \ \ \ \|\ \ \ \ \|<br/>
> |\ \ \ \ \ \|---------------------------------+\ \ \ \ \|<br/>
> |\ \ \ \ \ \|\ \ \ \ \ \ \ \ \ \communicate layer\ \ \ \ \ \ \ \ \ \ \ \ \|<br/>
> +-----|--------------------------------------+<br/>
> |\ \zsi\ \|\ \select\ \|\ \epoll\ \|\ \state-threads\ \|\ iocp|<br/>
> +--------------------------------------------+<br/>
> |\ Linux(CentOS/Ubuntu)\ \ \ \ \|\ \ \ \ Windows\ \ \ \ \ \ \ \|<br/>
> +--------------------------------------------+<br/>
 

### Building

There are several ways to incorporate znt into your project.

#### Linux

##### makefile

build release and install version

```
configure
make
sudo make install
```

build debug and install version

```
configure
make debug
sudo make debug install
```


#### Windows

visual studio project in build/vs2015
visual studio project in build/vs2017

## Interface

# Enjoy znt

- Z.Riemann (original author)
