pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                dir('build') {
                    deleteDir()
                    sh 'cmake .. -DCPACK_PACKAGE_CONTACT=jenkins@coruscate.nz -DCMAKE_BUILD_TYPE=Release'
                    sh 'cmake --build . --target package'
                }
            }
        }
        stage('Results') {
            steps {
                archiveArtifacts 'build/*.deb'
            }
        }
    }
}
