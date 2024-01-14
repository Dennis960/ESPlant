// @ts-check
import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

/**
 * TODO extract to class
 * @typedef {Object} Component
 * @property {THREE.Object3D<THREE.Object3DEventMap>} model
 * @property {string} name
 * @property {THREE.Mesh[]} meshes
 * @property {number} color
 * @property {function(number): void} setColor
 * @property {number} lineColor
 * @property {function(number): void} setLineColor
 * @property {boolean} highlighted
 * @property {boolean} selected
 * @property {boolean} highlightable
 * @property {boolean} selectable
 * @property {boolean} faceSelectable
 * @property {THREE.MeshStandardMaterial} material
 * @property {THREE.LineBasicMaterial} lineMaterial
 * Box mesh
 * @property {THREE.MeshStandardMaterial[]} boxMeshMaterials
 * @property {boolean[]} highlightedBoxMeshMaterials
 * @property {boolean[]} selectedBoxMeshMaterials
 * @property {THREE.Mesh} boxMesh
 * @property {number} boxMeshColor
 * @property {number} boxMeshOpacity
 * @property {number} boxMeshEdgeColor
 * @property {number} highlightedBoxMeshColor
 * @property {number} highlightedBoxMeshOpacity
 * @property {number} highlightedBoxMeshEdgeColor
 * @property {function(number): void} setBoxMeshColor
 * @property {function(number): void} setBoxMeshOpacity
 * @property {function(number): void} setBoxMeshEdgeColor
 * @property {function(number): void} setHighlightedBoxMeshColor
 * @property {function(number): void} setHighlightedBoxMeshOpacity
 * @property {function(number): void} setHighlightedBoxMeshEdgeColor
 */

export default class BoardRenderer {
    /**
     * Use release() to stop rendering and remove event listeners
     * 
     * @param {HTMLElement} container 
     */
    constructor(container) {
        this.container = container;
        this.scene = new THREE.Scene();
        this.renderer = new THREE.WebGLRenderer();

        const dirLight = new THREE.DirectionalLight(0xffffff, 1);
        dirLight.position.set(-10, -10, 30);
        this.scene.add(dirLight);

        const dirLight2 = new THREE.DirectionalLight(0xffffff, 3);
        dirLight2.position.set(10, 10, 30);
        this.scene.add(dirLight2);

        this.scene.background = new THREE.Color(0xffffff);

        this.camera = new THREE.PerspectiveCamera(75, container.clientWidth / container.clientHeight, 0.1, 1000);

        /**
         * @type {Component[]}
         */
        this.components = [];

        /**
         * @type {boolean}
         */
        this.areComponentsHighlightable = true;
        /**
         * @type {boolean}
         */
        this.areComponentsSelectable = true;

        this.defaultComponentColor = 0xffffff;
        this.defaultEdgeColor = 0x404040;
        this.selectedComponentColor = 0xFFFF00;
        this.highlightedEdgeColor = 0xb3b300;

        this.defaultBoxMeshColor = 0x000000;
        this.defaultBoxMeshOpacity = 0.2;
        this.defaultBoxMeshEdgeColor = 0xffff00;
        this.highlightedBoxMeshColor = 0x0000ff;
        this.highlightedBoxMeshOpacity = 0.6;
        this.highlightedBoxMeshEdgeColor = 0x000000;


        this.onMouseMove = this.onMouseMove.bind(this);
        this.onMouseClick = this.onMouseClick.bind(this);
        this.onMouseDown = this.onMouseDown.bind(this);
        this.onMouseUp = this.onMouseUp.bind(this);

        this.isRendering = false;
        this.isModelLoaded = false;

        /**
         * @type {boolean}
         */
        this.renderOnLoad = true;
    }

    centerModel() {
        // center the model
        const box = new THREE.Box3().setFromObject(this.model);
        const center = box.getCenter(new THREE.Vector3());
        this.model.position.sub(center);
    }

    centerCamera() {
        // ensure the model fits in the view
        const box = new THREE.Box3().setFromObject(this.model);
        const size = box.getSize(new THREE.Vector3());
        const maxDimension = Math.max(size.x, size.y, size.z);
        const fitHeightDistance = maxDimension / (2 * Math.atan(Math.PI * this.camera.fov / 360));
        const fitWidthDistance = fitHeightDistance / this.camera.aspect;
        const distance = Math.max(fitHeightDistance, fitWidthDistance);
        this.camera.position.z = distance;
    }

    /**
     * @param {string} glbPath 
     * @param {function(): void} onLoaded
     * @param {function(number): void} onProgress
     * @returns 
     */
    loadModel(glbPath, onLoaded, onProgress) {
        if (this.isModelLoaded) {
            return;
        }
        new GLTFLoader().load(glbPath, (gltf) => {
            /**
             * @type {THREE.Mesh}
             */
            this.model = gltf.scene;
            const board = this.model.children[0];
            for (let i = 0; i < board.children.length; i++) {
                const componentModel = board.children[i];

                // Create a bounding box for each component with individual faces
                const componentBox = new THREE.Box3().setFromObject(componentModel);
                const componentBoxSize = componentBox.getSize(new THREE.Vector3());
                const componentBoxCenter = componentBox.getCenter(new THREE.Vector3());
                const boxMeshMaterials = [
                    new THREE.MeshStandardMaterial({ color: this.defaultBoxMeshColor, transparent: true, opacity: this.defaultBoxMeshOpacity }), // right
                    new THREE.MeshStandardMaterial({ color: this.defaultBoxMeshColor, transparent: true, opacity: this.defaultBoxMeshOpacity }), // left
                    new THREE.MeshStandardMaterial({ color: this.defaultBoxMeshColor, transparent: true, opacity: this.defaultBoxMeshOpacity }), // top
                    new THREE.MeshStandardMaterial({ color: this.defaultBoxMeshColor, transparent: true, opacity: this.defaultBoxMeshOpacity }), // bottom
                    new THREE.MeshStandardMaterial({ color: this.defaultBoxMeshColor, transparent: true, opacity: this.defaultBoxMeshOpacity }), // front
                    new THREE.MeshStandardMaterial({ color: this.defaultBoxMeshColor, transparent: true, opacity: this.defaultBoxMeshOpacity }), // back
                ];
                const boxMesh = new THREE.Mesh(
                    new THREE.BoxGeometry(componentBoxSize.x, componentBoxSize.y, componentBoxSize.z),
                    boxMeshMaterials
                );
                boxMesh.position.copy(componentBoxCenter);
                const boxMeshEdges = new THREE.EdgesGeometry(boxMesh.geometry);
                const boxMeshEdgesMaterial = new THREE.LineBasicMaterial({ color: this.defaultBoxMeshEdgeColor });
                const boxMeshEdgesMesh = new THREE.LineSegments(boxMeshEdges, boxMeshEdgesMaterial);
                boxMeshEdgesMesh.position.copy(componentBoxCenter);
                componentModel.add(boxMeshEdgesMesh);
                this.scene.add(boxMesh);

                /**
                 * @type {Component}
                 */
                const component = {
                    model: componentModel,
                    name: componentModel.name,
                    meshes: componentModel.children[0].children,
                    color: this.defaultComponentColor,
                    setColor: function (color) {
                        this.color = color;
                        this.material.color.set(color);
                    },
                    lineColor: this.defaultEdgeColor,
                    setLineColor: function (color) {
                        this.lineColor = color;
                        this.lineMaterial.color.set(color);
                    },
                    highlighted: false,
                    selected: false,
                    highlightable: true,
                    selectable: true,
                    faceSelectable: true, // TODO set false
                    material: new THREE.MeshStandardMaterial({ color: this.defaultComponentColor }),
                    lineMaterial: new THREE.LineBasicMaterial({ color: this.defaultEdgeColor }),
                    boxMeshMaterials: boxMeshMaterials,
                    highlightedBoxMeshMaterials: [false, false, false, false, false, false],
                    selectedBoxMeshMaterials: [false, false, false, false, false, false],
                    boxMesh: boxMesh,
                    boxMeshColor: this.defaultBoxMeshColor,
                    boxMeshOpacity: this.defaultBoxMeshOpacity,
                    boxMeshEdgeColor: this.defaultBoxMeshEdgeColor,
                    highlightedBoxMeshColor: this.highlightedBoxMeshColor,
                    highlightedBoxMeshOpacity: this.highlightedBoxMeshOpacity,
                    highlightedBoxMeshEdgeColor: this.highlightedBoxMeshEdgeColor,
                    setBoxMeshColor: function (color) {
                        this.boxMeshColor = color;
                        this.boxMeshMaterials.forEach((material) => {
                            material.color.set(color);
                        });
                    },
                    setBoxMeshOpacity: function (opacity) {
                        this.boxMeshOpacity = opacity;
                        this.boxMeshMaterials.forEach((material) => {
                            material.opacity = opacity;
                        });
                    },
                    setBoxMeshEdgeColor: function (color) {
                        this.boxMeshEdgeColor = color;
                        this.boxMeshMaterials.forEach((material) => {
                            material.color.set(color);
                        });
                    },
                };

                this.components.push(component);

                for (const mesh of component.meshes) {
                    mesh.material = component.material;
                    const edges = new THREE.EdgesGeometry(mesh.geometry);
                    const edgesMesh = new THREE.LineSegments(edges, component.lineMaterial);
                    mesh.add(edgesMesh);
                }
            }

            this.centerCamera();

            this.scene.add(this.model);
            this.isModelLoaded = true;
            if (this.renderOnLoad) {
                this.render();
            }
            if (onLoaded !== undefined) {
                onLoaded();
            }
        }, (progressEvent) => {
            if (onProgress !== undefined) {
                onProgress(progressEvent.loaded / progressEvent.total);
            }
        }, (error) => {
            console.error(error);
            setTimeout(() => {
                this.loadModel(glbPath, onLoaded, onProgress);
            }, 1000); // retry
        });
    }

    render() {
        if (this.isRendering) {
            return;
        }
        this.isRendering = true;
        this.renderer.setPixelRatio(window.devicePixelRatio);
        this.renderer.setSize(this.container.clientWidth, this.container.clientHeight);
        this.container.innerHTML = '';
        this.container.appendChild(this.renderer.domElement);

        const controls = new OrbitControls(this.camera, this.renderer.domElement);

        const updateSize = () => {
            if (this.container.clientWidth !== this.renderer.domElement.width || this.container.clientHeight !== this.renderer.domElement.height) {
                this.renderer.setSize(this.container.clientWidth, this.container.clientHeight);
                this.camera.aspect = this.container.clientWidth / this.container.clientHeight;
                this.camera.updateProjectionMatrix();
            }
        }

        const animate = () => {
            updateSize();
            controls.update();
            this.renderer.render(this.scene, this.camera);
            if (this.isRendering)
                requestAnimationFrame(animate);
        }

        animate();

        this.mouseMovedAfterMouseDown = false;

        window.addEventListener('mousemove', this.onMouseMove, false);
        window.addEventListener('mousedown', this.onMouseDown, false);
        window.addEventListener('mouseup', this.onMouseUp, false);
    }

    /**
     * @param {{x: number, y: number}} windowPosition
     */
    getRaycastComponent(windowPosition) {
        const rect = this.container.getBoundingClientRect();
        const containerPosition = new THREE.Vector2(
            ((windowPosition.x - rect.left) / rect.width) * 2 - 1,
            -((windowPosition.y - rect.top) / rect.height) * 2 + 1
        )


        const raycaster = new THREE.Raycaster();
        raycaster.setFromCamera(containerPosition, this.camera);
        const intersects = raycaster.intersectObjects(this.components.map((component) => component.model), true);
        if (intersects.length > 0) {
            let intersectedObject = intersects[0].object;
            // Find the component that the intersected object belongs to, by going through all parent objects
            while (intersectedObject !== undefined && intersectedObject.parent !== null) {
                for (const component of this.components) {
                    if (component.model === intersectedObject) {
                        return component;
                    }
                }
                intersectedObject = intersectedObject.parent;
            }
        }
        return undefined;
    }

    /**
     * @param {{x: number, y: number}} windowPosition
     * @param {Component} component 
     * @returns {THREE.MeshStandardMaterial | undefined}
     * The material of the intersected face of the component's box mesh
     */
    getRaycastComponentBoxMeshMaterial(windowPosition, component) {
        const rect = this.container.getBoundingClientRect();
        const containerPosition = new THREE.Vector2(
            ((windowPosition.x - rect.left) / rect.width) * 2 - 1,
            -((windowPosition.y - rect.top) / rect.height) * 2 + 1
        )

        function normalToMeshMaterialIndex(normal) {
            if (normal === undefined) {
                return undefined;
            }
            if (normal.x === 1) {
                return 0;
            } else if (normal.x === -1) {
                return 1;
            } else if (normal.y === 1) {
                return 2;
            } else if (normal.y === -1) {
                return 3;
            } else if (normal.z === 1) {
                return 4;
            } else if (normal.z === -1) {
                return 5;
            }
        }

        const raycaster = new THREE.Raycaster();
        raycaster.setFromCamera(containerPosition, this.camera);
        const intersects = raycaster.intersectObject(component.boxMesh, true);
        if (intersects.length > 0) {
            let intersect = intersects[0];
            if (intersect !== null && intersect !== undefined) {
                const meshMaterialIndex = normalToMeshMaterialIndex(intersect.face?.normal);
                if (meshMaterialIndex !== undefined) {
                    return component.boxMeshMaterials[meshMaterialIndex];
                }
            }
        }
        return undefined;
    }

    onMouseMove(event) {
        this.mouseMovedAfterMouseDown = true;
        const mousePosition = {
            x: event.clientX,
            y: event.clientY
        };

        const component = this.getRaycastComponent(mousePosition);
        if (component?.faceSelectable) {
            const componentBoxMeshMaterial = this.getRaycastComponentBoxMeshMaterial(mousePosition, component);
            this.container.dispatchEvent(new CustomEvent('component-face-hover', {
                detail: {
                    component: component,
                    material: componentBoxMeshMaterial,
                },
            }));
        }
        if (component !== undefined) {
            if (this.areComponentsHighlightable) {
                if (!component.highlighted && component.highlightable) {
                    component.highlighted = true;
                    component.lineMaterial.color.set(this.highlightedEdgeColor);
                    this.container.dispatchEvent(new CustomEvent('component-highlight', {
                        detail: component,
                    }));
                }
            }
        }
        for (const otherComponent of this.components) {
            if (otherComponent !== component && otherComponent.highlighted) {
                otherComponent.highlighted = false;
                otherComponent.lineMaterial.color.set(otherComponent.lineColor);
                this.container.dispatchEvent(new CustomEvent('component-unhighlight', {
                    detail: component,
                }));
            }
        }
    }

    onMouseClick(event) {
        const mousePosition = {
            x: event.clientX,
            y: event.clientY
        };

        const component = this.getRaycastComponent(mousePosition);
        if (component !== undefined) {
            this.container.dispatchEvent(new CustomEvent('component-click', {
                detail: component,
            }));

            if (this.areComponentsSelectable) {
                if (!component.selected && component.selectable) {
                    component.selected = true;
                    component.material.color.set(this.selectedComponentColor);
                    this.container.dispatchEvent(new CustomEvent('component-select', {
                        detail: component,
                    }));
                } else if (component.selected) {
                    component.selected = false;
                    component.material.color.set(component.color);
                    this.container.dispatchEvent(new CustomEvent('component-unselect', {
                        detail: component,
                    }));
                }
            }
        }
    }

    onMouseDown() {
        this.mouseMovedAfterMouseDown = false;
    }

    onMouseUp(event) {
        if (!this.mouseMovedAfterMouseDown) {
            this.onMouseClick(event);
        }
    }

    /**
     * @returns {Component[]}
     */
    get selectedComponents() {
        return this.components.filter((component) => component.selected);
    }

    resetSelection() {
        for (const component of this.components) {
            if (component.selected) {
                component.selected = false;
                component.material.color.set(component.color);
                component.lineMaterial.color.set(component.lineColor);
            }
        }
    }

    release() {
        window.removeEventListener('mousemove', this.onMouseMove, false);
        window.removeEventListener('mousedown', this.onMouseDown, false);
        window.removeEventListener('mouseup', this.onMouseUp, false);
        this.isRendering = false;
    }
}
