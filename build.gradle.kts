plugins {
    id("com.android.library")
}

android {
    namespace = "net.ajcloud.imageprocessing"
    compileSdk = androidInt("compileSdkVersion")

    defaultConfig {
        minSdk = androidInt("minSdkVersion")
        consumerProguardFiles("consumer-rules.pro")
        externalNativeBuild {
            cmake {
                cppFlags += ""
                abiFilters += listOf("armeabi-v7a", "arm64-v8a")
                // 适配16K对齐
                arguments += "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON"
            }
        }
    }

    buildTypes {
        debug {}
        release {
            consumerProguardFiles("consumer-rules.pro")
        }
    }

    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
}

dependencies {
}
