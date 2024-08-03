@echo off
gltfpack -vpn -i %1 -o %~n1_opt.gltf && gltf2custom %~n1_opt.gltf %~n1.model
