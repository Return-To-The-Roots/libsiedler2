dockerRegistry = "git.ra-doersch.de:5005/"
dockerCredentials = "2d20af83-9ebd-4a5a-b6ee-c77bec430970"
dockerImage = "rttr/cross-compiler/linux/linux-amd64-docker:master"

pipeline {
    agent {
        label "docker"
    }
    
    options {
        buildDiscarder(logRotator(artifactDaysToKeepStr: '', artifactNumToKeepStr: '14', daysToKeepStr: '14', numToKeepStr: '180'))
        disableConcurrentBuilds()
    }

    environment {
        DEBIAN_FRONTEND = 'noninteractive'
    }

    stages {
        stage('checkout') {
            steps {
                dir('libutil') {
                    checkout scm: [
                        $class: 'GitSCM',
                        branches: [[name: '*/master']],
                        doGenerateSubmoduleConfigurations: false,
                        extensions: [
                            [$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: true, reference: '', trackingSubmodules: false],
                            [$class: 'GitLFSPull'],
                            [$class: 'AuthorInChangelog'],
                            [$class: 'CleanCheckout']
                        ],
                        submoduleCfg: [],
                        userRemoteConfigs: [[name: 'libutil', url: 'https://github.com/Return-To-The-Roots/libutil.git']]
                    ]
                }
                dir('libendian') {
                    checkout scm: [
                        $class: 'GitSCM',
                        branches: [[name: '*/master']],
                        doGenerateSubmoduleConfigurations: false,
                        extensions: [
                            [$class: 'SubmoduleOption', disableSubmodules: false, parentCredentials: true, recursiveSubmodules: true, reference: '', trackingSubmodules: false],
                            [$class: 'GitLFSPull'],
                            [$class: 'AuthorInChangelog'],
                            [$class: 'CleanCheckout']
                        ],
                        submoduleCfg: [],
                        userRemoteConfigs: [[name: 'libendian', url: 'https://github.com/Return-To-The-Roots/libendian.git']]
                    ]
                }
            }
        }
        stage('build') {
            steps {
                withDockerRegistry( registry: [credentialsId: dockerCredentials, url: 'https://'+dockerRegistry ] ) {
                    withDockerContainer(
                        image: dockerRegistry+dockerImage,
                        args: " \
                            -v $HOME/.ccache:/home/jenkins/.ccache \
                        ") {
                        sh """set -ex
                            rm -rf build || true
                            export LIBUTIL_DIR="\$(pwd)/libutil"
                            export LIBENDIAN_DIR="\$(pwd)/libendian"
                            export TRAVIS_OS_NAME=Unix
                            tools/travis-ci/build.sh Debug ""
                        """
                    }
                }
            }
        }
    }
    post {
        failure {
            mail to: 'sf-team@siedler25.org',
                subject: "Failed Pipeline: ${currentBuild.fullDisplayName}",
                body: "Pipeline failed: ${env.BUILD_URL}"
        }
        cleanup {
            sh 'rm -rf build'
        }
}}
