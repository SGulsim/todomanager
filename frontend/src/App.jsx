import { useState, useEffect } from 'react';
import Login from './components/Login';
import TaskList from './components/TaskList';
import { authAPI, tasksAPI } from './services/api';
import './App.css';

function App() {
	const [isAuthenticated, setIsAuthenticated] = useState(false);
	const [user, setUser] = useState(null);
	const [tasks, setTasks] = useState([]);
	const [loading, setLoading] = useState(true);

	useEffect(() => {
		if (authAPI.isAuthenticated()) {
			setUser(authAPI.getCurrentUser());
			setIsAuthenticated(true);
			loadTasks();
		} else {
			setLoading(false);
		}
	}, []);

	const loadTasks = async () => {
		try {
			const tasksData = await tasksAPI.getAll();
			setTasks(tasksData);
		} catch (error) {
			console.error('Failed to load tasks:', error);
		} finally {
			setLoading(false);
		}
	};

	const handleLogin = (userData) => {
		setUser(userData);
		setIsAuthenticated(true);
		loadTasks();
	};

	const handleLogout = () => {
		authAPI.logout();
		setIsAuthenticated(false);
		setUser(null);
		setTasks([]);
	};

	const handleCreateTask = async (taskData) => {
		try {
			const newTask = await tasksAPI.create(taskData);
			setTasks([...tasks, newTask]);
		} catch (error) {
			console.error('Failed to create task:', error);
			alert('Не удалось создать задачу');
		}
	};

	const handleUpdateTask = async (taskId, taskData) => {
		try {
			const updatedTask = await tasksAPI.update(taskId, taskData);
			setTasks(tasks.map((task) => (task.id === taskId ? updatedTask : task)));
		} catch (error) {
			console.error('Failed to update task:', error);
			alert('Не удалось обновить задачу');
		}
	};

	const handleDeleteTask = async (taskId) => {
		try {
			await tasksAPI.delete(taskId);
			setTasks(tasks.filter((task) => task.id !== taskId));
		} catch (error) {
			console.error('Failed to delete task:', error);
			alert('Не удалось удалить задачу');
		}
	};

	if (loading) {
		return (
			<div className='loading-container'>
				<div className='loading-spinner'>Загрузка...</div>
			</div>
		);
	}

	if (!isAuthenticated) {
		return <Login onLogin={handleLogin} />;
	}

	return (
		<div className='app'>
			<header className='app-header'>
				<div className='header-content'>
					<h1>Менеджер задач</h1>
					<div className='user-info'>
						<span>Добро пожаловать, {user?.username || 'Пользователь'}!</span>
						<button onClick={handleLogout} className='btn-logout'>
							Выйти
						</button>
					</div>
				</div>
			</header>

			<main className='app-main'>
				<TaskList
					tasks={tasks}
					onCreate={handleCreateTask}
					onUpdate={handleUpdateTask}
					onDelete={handleDeleteTask}
				/>
			</main>
		</div>
	);
}

export default App;
