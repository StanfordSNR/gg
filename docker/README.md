# Docker configuration

We have a several Docker configurations.

You need `docker` and `docker-compose` installed (for MacOS just use [the official app](https://docs.docker.com/engine/installation/mac/)).

## Setup

```sh
# clone and go to gg
git clone https://github.com/StanfordSNR/gg.git
cd gg

# build containers
docker-compose build
```

To continue, you should set environment variables to access AWS. One way is to create a `.env` file at the root of the project. Read more about these environment variables [here](https://github.com/StanfordSNR/gg#usage).

Here is an example:

```sh
# cat .env
GG_LAMBDA_ROLE=arn:aws:iam::238398044701:role/gg
GG_STORAGE_URI=s3://gg-ruby/?region=eu-north-1
AWS_ACCESS_KEY_ID=sXZaZmYdssmxviw
AWS_SECRET_ACCESS_KEY=msKDhYhwQeQZwvh/TOas6TO/dpIYn8n
AWS_REGION=eu-north-1
```

```sh
# compile gg
docker-compose run --rm runner -c ./docker/bin/compile

# setup serverless functions
docker-compose run --rm runner -c ./docker/bin/ggfunctions

# GEN      ggfunctions
# Installing lambda function gg-lambda-function... arn:aws:lambda:eu-north-1:12345678:function:gg-lambda-function
# done.
# Installing lambda function gg-meow-function... arn:aws:lambda:eu-north-1:12345678:function:gg-meow-function
# done.
```

### Use

After successful completion of the commands above you can use `gg`

```sh
docker-compose run --rm runner -c /bin/bash
root@88d653b86453:/app/gg# gg
gg COMMAND [ARG]...
```

**TIP**: Docker doesn't cleanup after itself well, so you have to do it manually.

```sh
# to monitor disk usage
docker system df

# to stop and clean application related containers
docker-compose down

# and with volumes (useful to reset databases state)
docker-compose down --volumes

# or even clean the whole docker system (it'll affect other applications too!)
docker system prune
```

**TIP2**: Use `--rm` flag with `run` command to automatically destroy the
container on exit, e.g. `docker-compose run --rm runner`.

**TIP3**: Out of the box `Dockerfile` contains two stages:
- `build-env` provides a minimal environment to build & run `gg` framework. Used for `TravisCI`.
- `sandbox-env` provides a container with all dependencies to play with `gg`'s examples.

If you are interested in other behaviors, you can use [multi-stage build](https://docs.docker.com/develop/develop-images/multistage-build) feature to build your own containers.
