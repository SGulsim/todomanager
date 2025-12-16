import './TaskItem.css';
import { formatDate, isOverdue } from '../utils/dateUtils';

const TaskItem = ({ task, onEdit, onDelete, onToggleStatus }) => {
	const getPriorityClass = (priority) => {
		switch (priority) {
			case 'high':
				return 'priority-high';
			case 'medium':
				return 'priority-medium';
			case 'low':
				return 'priority-low';
			default:
				return 'priority-medium';
		}
	};

	const getStatusClass = (status) => {
		switch (status) {
			case 'completed':
				return 'status-completed';
			case 'in_progress':
				return 'status-in-progress';
			default:
				return 'status-pending';
		}
	};

	const getPriorityLabel = (priority) => {
		switch (priority) {
			case 'high':
				return 'Высокий';
			case 'medium':
				return 'Средний';
			case 'low':
				return 'Низкий';
			default:
				return priority;
		}
	};

	const getStatusLabel = (status) => {
		switch (status) {
			case 'completed':
				return 'Выполнено';
			case 'in_progress':
				return 'В процессе';
			case 'pending':
				return 'В ожидании';
			default:
				return status.replace('_', ' ');
		}
	};

	const overdue = isOverdue(task.due_date, task.status, task.created_at);

	// Функция для форматирования текста - добавляет пробелы между словами если их нет
	const formatText = (text) => {
		if (!text) return '';
		// Если текст уже содержит пробелы, возвращаем как есть
		if (/\s/.test(text)) return text;
		
		// Если текст склеен, пытаемся добавить пробелы:
		// 1. Перед заглавными буквами (для camelCase)
		// 2. Перед цифрами после букв
		// 3. После цифр перед буквами
		// 4. Между русскими и английскими буквами
		let formatted = text
			.replace(/([а-яёa-z])([А-ЯЁA-Z])/g, '$1 $2') // пробел перед заглавной
			.replace(/([а-яёa-zА-ЯЁA-Z])(\d)/g, '$1 $2') // пробел перед цифрой после буквы
			.replace(/(\d)([а-яёa-zА-ЯЁA-Z])/g, '$1 $2') // пробел после цифры перед буквой
			.replace(/([а-яёА-ЯЁ])([a-zA-Z])/g, '$1 $2') // пробел между русскими и английскими
			.replace(/([a-zA-Z])([а-яёА-ЯЁ])/g, '$1 $2'); // пробел между английскими и русскими
		
		return formatted;
	};

	return (
		<div
			className={`task-item ${getPriorityClass(task.priority)} ${getStatusClass(
				task.status
			)} ${overdue ? 'overdue' : ''}`}
		>
			<div className='task-header'>
				<div className='task-title-row'>
					<input
						type='checkbox'
						checked={task.status === 'completed'}
						onChange={() => onToggleStatus(task.id)}
						className='task-checkbox'
					/>
					<h3 className={task.status === 'completed' ? 'completed' : ''}>
						{formatText(task.title)}
					</h3>
				</div>
				<div className='task-actions'>
					<button onClick={() => onEdit(task)} className='btn-edit'>
						Редактировать
					</button>
					<button onClick={() => onDelete(task.id)} className='btn-delete'>
						Удалить
					</button>
				</div>
			</div>

			{task.description && (
				<p className='task-description'>{formatText(task.description)}</p>
			)}

			<div className='task-meta'>
				<span className={`priority-badge ${getPriorityClass(task.priority)}`}>
					{getPriorityLabel(task.priority)}
				</span>
				<span className={`status-badge ${getStatusClass(task.status)}`}>
					{getStatusLabel(task.status)}
				</span>
				{task.created_at && task.created_at.trim() && (
					<span className='created-date'>
						Создано: {formatDate(task.created_at)}
					</span>
				)}
				{task.due_date && task.due_date.trim() && (
					<span className={`due-date ${overdue ? 'overdue-date' : ''}`}>
						Срок: {formatDate(task.due_date)}
						{overdue && <span className='overdue-badge'>Просрочено</span>}
					</span>
				)}
			</div>
		</div>
	);
};

export default TaskItem;
