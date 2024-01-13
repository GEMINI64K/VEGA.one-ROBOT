
function renderPointCloud()
    try     
        % Retrieve point cloud data from the function
        [x, y, z] = pointCloudData();

        % Visualize the point cloud with higher resolution
        figure;
        scatter3(x, y, z, 2, z, 'filled'); 
        xlabel('X');
        
        ylabel('Y');
        zlabel('Z');
        title('Point Cloud Visualization with Zoom');
        grid on;

        numPoints = length(z);
        fprintf('Number of points rendered: %d\n', numPoints);
    catch ME
        fprintf('Error occurred: %s\n', ME.message);
    end
end
