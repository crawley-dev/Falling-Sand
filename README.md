<a name="readme-top"></a>
<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/TheCookies/Falling-Sand">
<!--     <img src="images/logo.png" alt="Logo" width="80" height="80"> -->
  </a>

<h3 align="center">A Cellular Automata Physics Simulation.</h3>
  <p align="center">
    <div style="text-align: left; padding: 0px;">
      <div style="text-align: left; display: inline-block; padding-right: 10px;">
        <h4>Includes:</h4>   
        <p>
            - Falling Sand Simulation
            <br />
            - Conway's Game of Life
        </p>
      </div>
    </div>
    <!--<br />-->
    <!--<a href="https://github.com/github_username/repo_name"><strong>Explore the docs »</strong></a>-->
    <!--<br />-->
    <br />
    <!--<a href="https://github.com/github_username/repo_name">View Demo</a>-->
    <!--·-->
    <a href="https://github.com/TheCookiess/Falling-Sand/issues">Report Bug</a>
    ·
    <a href="https://github.com/TheCookiess/Falling-Sand/issues">Request Feature</a>
  </p>
</div>


<!-- TABLE OF CONTENTS -->
<div style="text-align: center;">
<details>
  <summary>Table of Contents</summary>
  <ol style="text-align: center; display: inline-block;">
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>
</div>

<!-- ABOUT THE PROJECT -->
## About The Project

![demo-screenshot]

<p>
  <i> Current Active Branch of Development: </i> 
  <b> chunked-world </b>
  <br /> <br />
  A C++ 'Falling Sand' simulation with Conway's Game of Life.
  <br /> 
  Inspired By: Noita's Falling sand, Sandspiel & The Powder Toy
</p>

<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Uses
* SDL2
* ImGui
* OpenGL
* Boost
* HIC++

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started

### Prerequisites to build
#### Install these libraries and place them in ./lib with these names:
- boost (version 1.82.0 or greater) >> https://www.boost.org/users/download/
- imgui (version 1.89 or greater) >> https://github.com/ocornut/imgui/releases
- SDL2 (version 2.0.0 or greater) >> https://github.com/libsdl-org/SDL/releases
- SDL2_IMAGE (version 2.0.0 or greater) >> https://github.com/libsdl-org/SDL_image/releases


### Installation
<!-- talk about installing boost at some point... -->

1. Place pre-requisite libraries ./lib
2. Navigate to ./premake
3. Run platform build script
4. Compile the project (e.g visual studio)
5. Navigate to ./bin/BUILD_SETTING-PLATFORM-ARCHITECTURE
6. Place library .dll files into the app
7. Run the application (e.g .exe)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
<!--## Usage-->

<!--Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.-->

<!--<p align="right">(<a href="#readme-top">back to top</a>)</p>-->



<!-- ROADMAP -->
## Roadmap

- [ ] Vector Acceleration
    - Re-write cell structure
    - euclids algorithm or RK4?
- [ ] Saving and Loading game states
    - [ ] multiple file formats
    - [ ] Loading and Saving as an image
- [ ] Multi-threading
    - [ ] Column Multi-Threading
    - [ ] Grid Multi-Threading
- [ ] Sleeping Chunks Optimisation
- [ ] Infinite World

See the [open issues](https://github.com/github_username/repo_name/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- Acknowledgements -->
## Acknowledgements

* [Github Readme Template](https://github.com/othneildrew/Best-README-Template/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[demo-screenshot]: resources/images/falling-sand-demo.png
[contributors-shield]: https://img.shields.io/github/contributors/TheCookiess/Falling-Sand.svg?style=for-the-badge
[contributors-url]: https://github.com/TheCookiess/Falling-Sand/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/TheCookiess/Falling-Sand.svg?style=for-the-badge
[forks-url]: https://github.com/TheCookiess/Falling-Sand/network/members
[stars-shield]: https://img.shields.io/github/stars/TheCookiess/Falling-Sand.svg?style=for-the-badge
[stars-url]: https://github.com/TheCookiess/Falling-Sand/stargazers
[issues-shield]: https://img.shields.io/github/issues/TheCookiess/Falling-Sand.svg?style=for-the-badge
[issues-url]: https://github.com/TheCookiess/Falling-Sand/issues
[license-shield]: https://img.shields.io/github/license/TheCookiess/Falling-Sand.svg?style=for-the-badge
[license-url]: https://github.com/TheCookiess/Falling-Sand/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/tom-crawley-30910b273/
[product-screenshot]: images/screenshot.png 
[readme-template]: https://github.com/othneildrew/Best-README-Template/
