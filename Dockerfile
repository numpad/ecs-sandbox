FROM debian:latest

# Install all dev dependencies
RUN apt-get update \
	&& apt-get upgrade \
	&& apt-get install -y git cmake ninja-build curl zip unzip tar g++ \
						  pkg-config libluajit-5.1-dev python3 libxinerama-dev \
						  libxcursor-dev xorg-dev libglu1-mesa-dev

# Install vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git
RUN ./vcpkg/bootstrap-vcpkg.sh
RUN ./vcpkg/vcpkg update \
	&& ./vcpkg/vcpkg install cereal glfw3 entt openal-soft glm lua sol2 assimp freetype --triplet=x64-linux \
	&& ./vcpkg/vcpkg integrate install

# Create the folder
RUN mkdir synthlike
COPY . /synthlike
WORKDIR /synthlike

CMD ["./scripts/build.sh"]
