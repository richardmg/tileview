import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

import QtQuick3D.TileView

Window {
    id: window
    width: 1024
    height: 640
    visible: true
    color: "#848895"

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: Qt.rgba(0, 0, 0.05, 1) }
            GradientStop { position: 1.0; color: Qt.rgba(0.15, 0.15, 0.15, 1) }
        }

        View3D {
            id: mainView
            anchors.fill: parent
            camera: personCamera
            importScene: scene
            environment: SceneEnvironment {
                //            backgroundMode: SceneEnvironment.SkyBox
                //            clearColor: Qt.rgba(0.1, 0.1, 0.1, 1.0)
                //            backgroundMode: SceneEnvironment.Color
                probeOrientation: Qt.vector3d(0, -90, 0)
                lightProbe: Texture {
                    source: "maps/OpenfootageNET_garage-1024.hdr"
                }
            }
        }
    }

    Node {
        id: scene

        TileView {
            id: tileView
            center: personCamera.position
            tileSize: Qt.vector3d(800, 800, 800)
            tileCount: Qt.vector3d(4, 4, 4)

            delegate: Node {
                id: delegate

                TileView.onTileChanged: {
                    anim.duration = Math.random(10000) + Math.abs(10000 * (TileView.tile.x + TileView.tile.z))
                    anim.restart()
                }

                Star {
                    id: star
                    instancing: randomInstancing
                    PropertyAnimation {
                        id: anim
                        target: star
                        loops: Animation.Infinite
                        property: "eulerRotation"
                        from: Qt.vector3d(0, 0, 0)
                        to: Qt.vector3d(360, 360, 360)
                    }
                }

                RandomInstancing {
                    id: randomInstancing
                    instanceCount: delegate.parent.tileSize.x / 3
                    position: InstanceRange {
                        from: Qt.vector3d(delegate.parent.tileSize.x / 2,
                                          delegate.parent.tileSize.y / 2,
                                          delegate.parent.tileSize.z / 2)
                        to: from.times(-1)
                    }
                }
            }

            Connections {
                target: personCamera
                function onRotationChanged() { tileView.direction = personCamera.forward }
            }
            Component.onCompleted: direction = personCamera.forward
        }

        PerspectiveCamera {
            id: personCamera
            position: Qt.vector3d(0, 0, 0)
            Model {
                source: "#Cone"
                scale: Qt.vector3d(0.5, 0.5, 0.5)
                eulerRotation: Qt.vector3d(-90, 0, 0)
                materials: [
                    DefaultMaterial {
                        diffuseColor: "red"
                    }
                ]
            }
        }

    }

    WasdController {
        controlledObject: personCamera
    }

}
