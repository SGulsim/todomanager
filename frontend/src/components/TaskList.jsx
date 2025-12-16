import { useState, useMemo } from 'react';
import TaskItem from './TaskItem';
import TaskForm from './TaskForm';
import { isOverdue, isToday, isThisWeek } from '../utils/dateUtils';
import './TaskList.css';

const TaskList = ({ tasks, onUpdate, onDelete, onCreate }) => {
	const [showForm, setShowForm] = useState(false);
	const [editingTask, setEditingTask] = useState(null);
	const [filter, setFilter] = useState('all');
	const [dateFilter, setDateFilter] = useState('all');
	const [sortBy, setSortBy] = useState('priority');
	const [sortOrder, setSortOrder] = useState('desc');
	const [searchQuery, setSearchQuery] = useState('');

	const filteredAndSortedTasks = useMemo(() => {
		let filtered = tasks;

		if (filter === 'completed') {
			filtered = filtered.filter((task) => task.status === 'completed');
		} else if (filter === 'pending') {
			filtered = filtered.filter((task) => task.status !== 'completed');
		}

		if (dateFilter === 'overdue') {
			filtered = filtered.filter((task) =>
				isOverdue(task.due_date, task.status, task.created_at)
			);
		} else if (dateFilter === 'today') {
			filtered = filtered.filter(
				(task) => task.due_date && isToday(task.due_date)
			);
		} else if (dateFilter === 'thisWeek') {
			filtered = filtered.filter(
				(task) => task.due_date && isThisWeek(task.due_date)
			);
		} else if (dateFilter === 'noDate') {
			filtered = filtered.filter(
				(task) => !task.due_date || !task.due_date.trim()
			);
		}

		if (searchQuery) {
			filtered = filtered.filter((task) =>
				task.title.toLowerCase().includes(searchQuery.toLowerCase())
			);
		}

		const sorted = [...filtered].sort((a, b) => {
			let result = 0;
			
			switch (sortBy) {
				case 'priority':
					const priorityOrder = { high: 3, medium: 2, low: 1 };
					const priorityA = priorityOrder[a.priority] || 2;
					const priorityB = priorityOrder[b.priority] || 2;
					result = priorityB - priorityA; // По умолчанию: высокий приоритет первым
					// Если приоритеты одинаковые, сортируем по дате создания
					if (result === 0) {
						result = new Date(b.created_at || 0) - new Date(a.created_at || 0);
					}
					break;
				case 'title':
					result = a.title.localeCompare(b.title);
					break;
				case 'date':
				default:
					result = new Date(b.created_at || 0) - new Date(a.created_at || 0);
					break;
			}
			
			// Применяем порядок сортировки (asc/desc)
			return sortOrder === 'asc' ? -result : result;
		});

		return sorted;
	}, [tasks, filter, dateFilter, sortBy, sortOrder, searchQuery]);

	const handleEdit = (task) => {
		setEditingTask(task);
		setShowForm(true);
	};

	const handleDelete = async (taskId) => {
		if (window.confirm('Вы уверены, что хотите удалить эту задачу?')) {
			await onDelete(taskId);
		}
	};

	const handleToggleStatus = async (taskId) => {
		const task = tasks.find((t) => t.id === taskId);
		if (task) {
			const newStatus = task.status === 'completed' ? 'pending' : 'completed';
			await onUpdate(taskId, { ...task, status: newStatus });
		}
	};

	const handleFormSave = async (taskData) => {
		if (editingTask) {
			await onUpdate(editingTask.id, { ...editingTask, ...taskData });
		} else {
			await onCreate(taskData);
		}
		setShowForm(false);
		setEditingTask(null);
	};

	const handleFormCancel = () => {
		setShowForm(false);
		setEditingTask(null);
	};

	return (
		<div className='task-list-container'>
			<div className='task-list-header'>
				<h1>Мои задачи</h1>
				<button onClick={() => setShowForm(true)} className='btn-add-task'>
					+ Добавить задачу
				</button>
			</div>

			<div className='task-controls'>
				<div className='search-box'>
					<input
						type='text'
						placeholder='Поиск задач...'
						value={searchQuery}
						onChange={(e) => setSearchQuery(e.target.value)}
						className='search-input'
					/>
				</div>

				<div className='filters'>
					<select
						value={filter}
						onChange={(e) => setFilter(e.target.value)}
						className='filter-select'
					>
						<option value='all'>Все задачи</option>
						<option value='pending'>Невыполненные</option>
						<option value='completed'>Выполненные</option>
					</select>

					<select
						value={dateFilter}
						onChange={(e) => setDateFilter(e.target.value)}
						className='filter-select'
					>
						<option value='all'>Все даты</option>
						<option value='overdue'>Просроченные</option>
						<option value='today'>Сегодня</option>
						<option value='thisWeek'>На этой неделе</option>
						<option value='noDate'>Без срока</option>
					</select>

					<select
						value={sortBy}
						onChange={(e) => setSortBy(e.target.value)}
						className='sort-select'
					>
						<option value='priority'>По приоритету</option>
						<option value='date'>По дате</option>
						<option value='title'>По названию</option>
					</select>
					
					{sortBy === 'priority' && (
						<select
							value={sortOrder}
							onChange={(e) => setSortOrder(e.target.value)}
							className='sort-order-select'
							title='Порядок сортировки'
						>
							<option value='desc'>Высокий → Низкий</option>
							<option value='asc'>Низкий → Высокий</option>
						</select>
					)}
				</div>
			</div>

			<div className='task-stats'>
				<span>Всего: {tasks.length}</span>
				<span>
					Выполнено: {tasks.filter((t) => t.status === 'completed').length}
				</span>
				<span>
					В ожидании: {tasks.filter((t) => t.status !== 'completed').length}
				</span>
			</div>

			<div className='tasks-grid'>
				{filteredAndSortedTasks.length === 0 ? (
					<div className='no-tasks'>
						<p>Задачи не найдены. Создайте свою первую задачу!</p>
					</div>
				) : (
					filteredAndSortedTasks.map((task) => (
						<TaskItem
							key={task.id}
							task={task}
							onEdit={handleEdit}
							onDelete={handleDelete}
							onToggleStatus={handleToggleStatus}
						/>
					))
				)}
			</div>

			{showForm && (
				<TaskForm
					task={editingTask}
					onSave={handleFormSave}
					onCancel={handleFormCancel}
				/>
			)}
		</div>
	);
};

export default TaskList;
