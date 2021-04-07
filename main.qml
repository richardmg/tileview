import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

import TileEngine

Window {
    id: window
    width: 1280
    height: 720
    visible: true
    color: "#848895"

    Row {
        anchors.fill: parent
        spacing: 10

        View3D {
            height: parent.height
            width: parent.width / 2
            camera: personCamera
            importScene: scene
        }

        View3D {
            height: parent.height
            width: parent.width / 2
            camera: droneCamera
            importScene: scene
        }
    }

    Node {
        id: scene

        PerspectiveCamera {
            id: personCamera
            position: Qt.vector3d(0, 0, 0)
            Model {
                source: "#Cone"
                scale: Qt.vector3d(0.1, 0.1, 0.1)
//                eulerRotation: Qt.vector3d(-90, 0, 0)
                materials: [
                    DefaultMaterial {
                        diffuseColor: "red"
                    }
                ]
            }
        }

        PerspectiveCamera {
            id: droneCamera
            position: Qt.vector3d(personCamera.x, 1000, personCamera.z)
            eulerRotation: Qt.vector3d(-90, 0, 0)
        }

        TileView {
            center: personCamera.position
            tileSize: 100
            tileCount: 4

            delegate: Model {
                source: "#Cube"
                scale: Qt.vector3d(0.5, 0.1, 0.5)
                materials: [
                    DefaultMaterial {
                        diffuseColor: "yellow"
                    }
                ]
            }
        }

        DirectionalLight {
            position: Qt.vector3d(-500, 500, -100)
            color: Qt.rgba(0.4, 0.2, 0.6, 1.0)
            ambientColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
        }

        PointLight {
            position: Qt.vector3d(0, 0, 100)
            color: Qt.rgba(0.1, 1.0, 0.1, 1.0)
            ambientColor: Qt.rgba(0.2, 0.2, 0.2, 1.0)
        }

        Model {
            scale: Qt.vector3d(0.1, 0.1, 0.1)
            source: "#Cube"
//            geometry: ExampleTriangleGeometry { uv: true }
            materials: [
                DefaultMaterial {
                    Texture {
                        id: baseColorMap
                        source: "qt_logo_rect.png"
                    }
                    cullMode: DefaultMaterial.NoCulling
                    diffuseMap: baseColorMap
//                    specularAmount: 0.5
//                    lineWidth: 3
                }
            ]
        }
    }

    WasdController {
        controlledObject: personCamera
    }

}
